#include "stdafx.h"
#include "CppUnitTest.h"
#include "../AnimateAnything/AnimateAnything.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestAnimateAnything
{
	TEST_CLASS(UnitTestAnimationNodes)
	{
	public:

		// An easy test for container
		TEST_METHOD(TestContainerBasic)
		{
			using namespace AnimateAnything;
			Container<double> container;
			double x = 0.0;
			auto anim = container.Between(0.5, 1.5, [&](double time) { x = time; });
			anim->PlaySimple(1.0);
			auto anim2 = container.Parallel(anim);
			anim2->PlaySimple(1.0);
			Assert::AreEqual(0.5, x, 0.1, L"This should work");
		}

		// See if container can build with 
		TEST_METHOD(TestContainerParallelCanProcessLambdas)
		{
			AnimateAnything::Container<double> container;
			double x = 0.0, y = 0.0;
			auto anim = container.Between(0.5, 1.5, [&](double time) { x = time; });
			auto anim2 = container.Parallel([&](double time) { y = time*2.0; }, anim);
			anim2->PlaySimple(1.0);
			Assert::AreEqual(0.5, x, 0.1, L"x should be set to 0.5 by between");
			Assert::AreEqual(2.0, y, 0.1, L"y should be set to 4.0 by labmda");
		}

		// test sequence with lambdas that process time
		TEST_METHOD(TestContainerAnimationSequence)
		{
			// init variables
			int x = 0;
			AnimateAnything::Container<double> aa;

			// build an animation
			auto anim = aa.Between(.0, 10.0,
				aa.Between(0, 2, [&](double time) { x = +1; }),
				aa.Between(2, 4, [&](double time) { x = -3-time; }),
				aa.Between(4, 6, [&](double time) { x = +8+time; }),
				aa.Between(6, 8, [&](double time) { x = -9; }),
				aa.Between(8, 10, [&](double time) { x = -11; })
			);

			// let's see if it works
			anim->PlaySimple(-1.0);
			Assert::AreEqual(0, x, L"x should not change.");
			anim->PlaySimple(11.0);
			Assert::AreEqual(0, x, L"x should not change.");
			anim->PlaySimple(1.0);
			Assert::AreEqual(+1, x);
			anim->PlaySimple(3.0);
			Assert::AreEqual(-3 - 1, x);
			anim->PlaySimple(5.0);
			Assert::AreEqual(+8 + 1, x);
			anim->PlaySimple(7.0);
			Assert::AreEqual(-9, x);
			anim->PlaySimple(9.0);
			Assert::AreEqual(-11, x);
		}

		// Test a sequence with lambdas that ignore time
		TEST_METHOD(TestContainerAnimationSequenceVoidAction)
		{
			// init variables
			int x = 0;
			AnimateAnything::Container<double> aa;

			// build an animation
			auto anim = aa.Between(.0, 10.0,
				aa.Between(0, 2, [&]() { x = +1; }),
				aa.Between(2, 4, [&]() { x = -3; }),
				aa.Between(4, 6, [&]() { x = +8; }),
				aa.Between(6, 8, [&]() { x = -9; }),
				aa.Between(8, 10, [&]() { x = -11; })
			);

			// let's see if it works
			anim->PlaySimple(-1.0);
			Assert::AreEqual(0, x, L"x should not change.");
			anim->PlaySimple(11.0);
			Assert::AreEqual(0, x, L"x should not change.");
			anim->PlaySimple(1.0);
			Assert::AreEqual(+1, x);
			anim->PlaySimple(3.0);
			Assert::AreEqual(-3, x);
			anim->PlaySimple(5.0);
			Assert::AreEqual(+8, x);
			anim->PlaySimple(7.0);
			Assert::AreEqual(-9, x);
			anim->PlaySimple(9.0);
			Assert::AreEqual(-11, x);
		}

	};
}
