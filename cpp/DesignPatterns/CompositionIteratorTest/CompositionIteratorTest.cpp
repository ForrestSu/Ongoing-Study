// 2017-04-26T19:41:20+08:00
#include <cassert>
#include <iostream>
#include <string>

#include "../../common.h"
#include "../Composite.hpp"

class Leaf : public Composite
{
public:
    Leaf(const char *text)
    {
        assert(text != nullptr);
        text_ = text;
    }

    Leaf(const std::string &text)
        : Leaf(text.c_str())
    {
    }

    Leaf(int id)
        : Leaf(std::to_string(id))
    {
    }

    std::string GetText() const { return text_; }

private:
    std::string text_;
};

int main()
{
    Leaf root(1);
        auto leaf2 = std::make_shared<Leaf>(2);
        root.Add(leaf2);
            auto leaf5 = std::make_shared<Leaf>(5);
            leaf2->Add(leaf5);
                leaf5->Add(std::make_shared<Leaf>(9));
                leaf5->Add(std::make_shared<Leaf>(10));
            leaf2->Add(std::make_shared<Leaf>(6));
        root.Add(std::make_shared<Leaf>(3));
        auto leaf4 = std::make_shared<Leaf>(4);
        root.Add(leaf4);
            auto leaf7 = std::make_shared<Leaf>(7);
                leaf7->Add(std::make_shared<Leaf>(11));
                leaf7->Add(std::make_shared<Leaf>(12));
            leaf4->Add(leaf7);
            leaf4->Add(std::make_shared<Leaf>(8));

#if 1
#if 0
    /*
    error C2672: 'Component::CreateTypedIterator': no matching overloaded function found
    error C2784: 'std::shared_ptr<Iterator<std::shared_ptr<_Ty>>> Component::CreateTypedIterator(TraversalKind,std::function<bool(std::shared_ptr<_Ty>)>)': could not deduce template argument for 'std::function<bool(std::shared_ptr<_Ty>)>' from 'main::<lambda_f7c688ea1e2e6188dc61a2174eb968b6>'
    */
    auto
#else
    std::function<bool(std::shared_ptr<Leaf>)>
#endif
        selector = [](std::shared_ptr<Leaf> leaf)
    {
        assert(leaf);
        return std::stoi(leaf->GetText()) % 2 == 0;
    };
#else
    std::function<bool(std::shared_ptr<Leaf>)> selector;
#endif

    // None test
    auto iterator = root.CreateTypedIterator<Leaf>(None, selector);
    for (iterator->First(); !iterator->IsDone(); iterator->MoveNext())
    {
        std::cout << iterator->Current()->GetText() << " ";
    }
    std::cout << std::endl;

    // BFS test
    iterator = root.CreateTypedIterator(BFS, selector);
    for (iterator->First(); !iterator->IsDone(); iterator->MoveNext())
    {
        std::cout << iterator->Current()->GetText() << " ";
    }
    std::cout << std::endl;

    // DFS test
    iterator = root.CreateTypedIterator(DFS, selector);
    for (iterator->First(); !iterator->IsDone(); iterator->MoveNext())
    {
        std::cout << iterator->Current()->GetText() << " ";
    }
    std::cout << std::endl;

    PAUSE();
    return 0;
}
