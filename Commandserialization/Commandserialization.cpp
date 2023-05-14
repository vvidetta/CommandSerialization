// Commandserialization.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <tuple>

class API
{
public:
    virtual ~API() = default;
    API(API const&) = delete;
    API& operator=(API const&) = delete;
    API(API&&) = delete;
    API& operator=(API&&) = delete;

    virtual int add(int a, int b) = 0;

    virtual int subtract(int a, int b) = 0;

    virtual void simple() = 0;

protected:
    API() = default;
};

class ISerializer
{
public:
    virtual ~ISerializer() = default;
    ISerializer(ISerializer const&) = delete;
    ISerializer& operator=(ISerializer const&) = delete;
    ISerializer(ISerializer&&) = delete;
    ISerializer& operator=(ISerializer&&) = delete;
};

class concreteAPI final : public API
{
public:
    int add(int a, int b) override
    {
        return a + b;
    }

    int subtract(int a, int b) override
    {
        return a - b;
    }

    void simple() override
    {
        printf("simple called\n");
    }
};

class ICommand
{
public:
    virtual ~ICommand() = default;
    ICommand(ICommand const&) = delete;
    ICommand& operator=(ICommand const&) = delete;
    ICommand(ICommand&&) = delete;
    ICommand& operator=(ICommand&&) = delete;

    virtual void execute() = 0;

    virtual bool hasResult() const noexcept = 0;

    virtual std::string serializeResult(ISerializer const& serializer) = 0;
protected:
    ICommand() = default;
};

template <
    typename R,
    typename... Args
>
class APICommand : public ICommand
{
public:
    using APIFunction = R(API::*)(Args...);

    explicit APICommand(APIFunction func, API& api, Args&&... args)
        : m_func{ func },
        m_args{ std::make_tuple<std::reference_wrapper<API>, Args...>(std::ref(api), args...) }
    { }

    void execute() override
    {
        std::apply(m_func, m_args);
    }

    bool hasResult() const noexcept override
    {
        return false;
    }

    std::string serializeResult(ISerializer const& serializer) override
    {
        return {};
    }

private:
    APIFunction m_func;
    std::tuple<std::reference_wrapper<API>, Args...> m_args;
};

template <
    typename... Args
>
class APICommand : public ICommand
{
public:
    using APIFunction = void(API::*)(Args...);

    explicit APICommand(APIFunction func, API& api, Args&&... args)
        : m_func{ func },
        m_args{ std::make_tuple<std::reference_wrapper<API>, Args...>(std::ref(api), args...) }
    { }

    void execute() override
    {
        std::apply(m_func, m_args);
    }

    bool hasResult() const noexcept override
    {
        return false;
    }

    std::string serializeResult(ISerializer const& serializer) override
    {
        return {};
    }

private:
    APIFunction m_func;
    std::tuple<std::reference_wrapper<API>, Args...> m_args;
};

int main()
{
    auto api = concreteAPI{};
    auto command = std::make_unique<APICommand<void>>(&API::simple, api);
    command->execute();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
