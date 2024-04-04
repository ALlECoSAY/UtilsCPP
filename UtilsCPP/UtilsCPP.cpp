
#include <iostream>



#include "Delegate.h"

struct Foo
{
	Foo() = delete;
	Foo(int aa) { a = aa; };
public:
		int a;
};

class Test
{
public:
	Test() {};

	Test(int a)
	{
	}

	void DoAnyting()
	{
		std::cout << "DoAnyting" << std::endl;
	}

	int Increment(int a)
	{
		return ++a;
	}

	std::optional<Foo> DoSmthgOptFoo(int a)
	{
		return Foo(a);
	}

	Foo DoSmthgFoo(int a)
	{
		return Foo(a);
	}

};

int main()
{
	std::shared_ptr<Test> spTest = std::make_shared<Test>(5);
	Delegate<void> delegate;

	delegate.BindLambda([]() {
			std::cout << "Lambda" << std::endl;
		});
	
	delegate.Execute();



	return 0;
}
