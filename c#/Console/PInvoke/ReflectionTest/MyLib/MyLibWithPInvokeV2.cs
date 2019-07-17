﻿// 2018-05-17T12:14+08:00
namespace MyLib
{
    using System.Runtime.InteropServices;

    using MyLibBase;

    public sealed class MyLibWithPInvokeV2 : MyLibBase
    {
        public MyLibWithPInvokeV2(string message)
        {
            message_ = message;
            name_ = NativeLib.GetName();
        }

        public override string Message => message_;

        public override string Name => name_;

        private readonly string message_;
        private readonly string name_;
    }
}
