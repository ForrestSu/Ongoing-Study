// 2014-12-17T13:31+08:00

// An example for testing Azure Storage CPP SDK. 
// Several official examples can be found here:
// https://github.com/Azure/azure-storage-cpp/blob/master/Microsoft.WindowsAzure.Storage/samples/

#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <locale>

#include <boost/filesystem.hpp>
#include <boost/progress.hpp>
#include <boost/property_tree/ini_parser.hpp>
//#include <boost/uuid/uuid.hpp>

#include <cpprest/filestream.h>

#include <wascore/basic_types.h>
#include <was/blob.h>
#include <was/common.h>
#include <was/storage_account.h>

#ifdef _MSC_VER
# ifdef NDEBUG
#  pragma comment(lib, "cpprest120_2_3.lib") // cpprest SDK for VS2013
# else
#  pragma comment(lib, "cpprest120d_2_3.lib") // cpprest SDK for VS2013
# endif

# pragma comment(lib, "wastorage.lib")
#endif

#ifdef _MSC_VER
# ifdef NDEBUG
#  define PAUSE() (__noop)
# else
#  define PAUSE() std::system("pause")
# endif
#else
# define PAUSE() ((void)0)
#endif

#define RETURN_ON_FAILURE(msg) std::cerr << msg << ": " << e.what() << '\n'; PAUSE(); return EXIT_FAILURE

namespace AS = azure::storage;
namespace Cfg = boost::property_tree;

int main(int argc, char *argv[])
{
    //std::locale::global(std::locale(""));

    // parse the configuration file and fetch out account name, account key, host endpoint
    Cfg::basic_ptree<utility::string_t, utility::string_t> cfg;
    try {
        // You can create your own configuration file based on "azure_storage_test.cfg.template".
        Cfg::read_ini("azure_storage_test.cfg", cfg);
    } catch (const Cfg::ini_parser_error &e) {
        RETURN_ON_FAILURE("Parsing configuration file failed:");
    }

    utility::string_t accountName;
    utility::string_t primaryAccessKey;
    utility::string_t secondaryAccessKey;
    // I am using Microsoft Azure China service, so my endpoint is: core.chinacloudapi.cn
    utility::string_t endPoint;
    bool useHttps;
    bool useDevAccount;

    try {
        accountName = cfg.get<utility::string_t>(U("ACCOUNT.account_name"));
        primaryAccessKey = cfg.get<utility::string_t>(U("ACCOUNT.primary_access_key"));
        secondaryAccessKey = cfg.get<utility::string_t>(U("ACCOUNT.secondary_access_key"));
        endPoint = cfg.get<utility::string_t>(U("ACCOUNT.end_point"));
        useHttps = cfg.get<bool>(U("ACCOUNT.use_https"));
        useDevAccount = cfg.get<bool>(U("ACCOUNT.use_dev_account"));
    } catch (const Cfg::ptree_bad_path &e) {
        RETURN_ON_FAILURE("Specified key not found");
    } catch (const Cfg::ptree_bad_data &e) {
        RETURN_ON_FAILURE("Bad data");
    } catch (const std::exception &e) {
        RETURN_ON_FAILURE("Fetching configuration information failed");
    }

    AS::cloud_storage_account asAccount;

    // initialize the storage account
#if 1
    try {
        if (useDevAccount) {
            // If you want to use a development storage account, please read this article: 
            // Using the Azure Storage Emulator for Development and Testing
            // http://msdn.microsoft.com/en-us/library/azure/hh403989.aspx
            asAccount = AS::cloud_storage_account::development_storage_account();
        } else {
            AS::storage_credentials credential(accountName, primaryAccessKey);
            asAccount = AS::cloud_storage_account(credential, endPoint, useHttps);
        }
    } catch (const std::exception &e) {
        RETURN_ON_FAILURE("Creating storage account object failed");
    }
#else
    // The workflow of parse:
    //   1. parse_devstore_settings
    //   2. parse_defaults_settings
    //   3. parse_explicit_settings
    // In cloud_storage_account.cpp, we can see all the valid setting strings.
    utility::string_t connStr;
    if (useDevAccount) {
        connStr = U("UseDevelopmentStorage=true;DevelopmentStorageProxyUri=;");
    } else {
        connStr =
            utility::string_t(U("DefaultEndpointsProtocol=")) + (useHttps ? U("https;") : U("http;")) +
            U("AccountName=") + accountName + U(";") +
            U("AccountKey=") + primaryAccessKey + U(";") +
            U("EndpointSuffix=") + endPoint + U(";");
    }

    try {
        asAccount = AS::cloud_storage_account::parse(connStr);
    } catch (const std::exception &e) {
        RETURN_ON_FAILURE("Parsing connection string failed");
    }
#endif

    AS::blob_request_options reqOptions;
    reqOptions.set_stream_read_size_in_bytes(1024 * 1024);
    reqOptions.set_stream_write_size_in_bytes(1024 * 1024);
    reqOptions.set_server_timeout(std::chrono::seconds(5));
    reqOptions.set_store_blob_content_md5(true);

    try {
        AS::cloud_blob_client blobClient = asAccount.create_cloud_blob_client(reqOptions);
        // Caution:
        // A container name must be a valid DNS name. Please read the naming convention described here:
        // http://msdn.microsoft.com/en-us/library/azure/dd135715.aspx
        AS::cloud_blob_container container = blobClient.get_container_reference(U("myd7349"));

        container.create_if_not_exists();

        // Set access permission
        AS::blob_container_permissions permission;
        permission.set_public_access(AS::blob_container_public_access_type::blob);
        container.upload_permissions(permission);

        // Upload a blob from a file
        //utility::string_t filePath = U("D:\\学习笔记.txt");
        utility::string_t filePath = U("D:\\20120929152243.dat");
        concurrency::streams::istream inStream
            = concurrency::streams::file_stream<std::uint8_t>::open_istream(filePath).get();
        AS::cloud_block_blob blob = container.get_block_blob_reference(boost::filesystem::path(filePath).filename().native());
        
        // Now, start the uploading process. If we want to upload a block each time, then:
        //   C#:     PutBlock, PutBlockList
        //   Python: put_blob
        //   C++:    upload_block, upload_block_list
        ucout << U("Uploading file: ") << filePath << U("...");
        {
            utility::size64_t fileSize = boost::filesystem::file_size(filePath);
            boost::progress_display prog(fileSize);
            boost::progress_timer t;

            while (fileSize >= reqOptions.stream_write_size_in_bytes()) {
                blob.upload_from_stream(inStream, reqOptions.stream_write_size_in_bytes());
                prog += reqOptions.stream_write_size_in_bytes();
                fileSize -= reqOptions.stream_write_size_in_bytes();
            }

            if (fileSize > 0) {
                blob.upload_from_stream(inStream, fileSize);
                prog += fileSize;
            }
        }

        // Close the stream
        inStream.close().wait();
    } catch (const AS::storage_exception &e) {
        RETURN_ON_FAILURE("Azure storage exception");
    } catch (const std::exception &e) {
        RETURN_ON_FAILURE("Unknown exception");
    }

    PAUSE();
    return EXIT_SUCCESS;
}

// References:
// [Understanding Block Blobs and Page Blobs](http://msdn.microsoft.com/en-us/library/azure/ee691964.aspx)
// [Windows Azure: 使用Blob的PutBlock实现大文件断点续传](http://www.cnblogs.com/lijiawei/archive/2013/01/18/2866756.html)
