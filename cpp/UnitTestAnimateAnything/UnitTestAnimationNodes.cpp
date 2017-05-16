#include "stdafx.h"
#include "CppUnitTest.h"
#include "../AnimateAnything/AnimateAnything.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestAnimateAnything
{		
	TEST_CLASS(UnitTestAnimationNodes)
	{
	public:

		using ActionVoid = AnimateAnything::AnimationActionVoid<double>;
		using Action = AnimateAnything::AnimationActionTime<double>;
		using After = AnimateAnything::AnimationAfter<double>;
		using Before = AnimateAnything::AnimationBefore<double>;
		using Between = AnimateAnything::AnimationBetween<double>;
		using Seek = AnimateAnything::AnimationSeek<double>;
		using Event = AnimateAnything::AnimationEvent<double>;
		using Stretch = AnimateAnything::AnimationStretch<double>;
		using TimeTransform = AnimateAnything::AnimationTimeTransform<double>;
		using Parallel = AnimateAnything::AnimationParallel<double>;

		TEST_METHOD(TestAnimationActionRunsLambdas)
		{
			double value = 0;
			Action animation ([&](double t) { value = t * 2.0; });
			animation.PlaySimple(1);
			Assert::AreEqual(2.0, value, 0.01, L"Value should be 2.0 (t*2)");
		}


		TEST_METHOD(TestAnimationActionRunsVoidLambdas)
		{
			double value = 0;
			ActionVoid animation([&]() { value = 2.0; });
			animation.PlaySimple(1);
			Assert::AreEqual(2.0, value, 0.01, L"Value should be 2.0 (t*2)");
		}



		TEST_METHOD(TestAnimationAfterConditionalExecution)
		{
			double value = 0;
			Action valueAnim([&](double t) { value = t * 2.0; });
			After animation(1.0, valueAnim);
			value = 667.0; animation.PlaySimple(0.0); Assert::AreEqual(667.0, value, 0.01, L"Value should not change.");
			value = 667.0; animation.PlaySimple(1.0); Assert::AreNotEqual(667.0, value, 0.01, L"Value should change.");
			value = 667.0; animation.PlaySimple(2.0); Assert::AreNotEqual(667.0, value, 0.01, L"Value should change.");
		}

		TEST_METHOD(TestAnimationBeforeConditionalExecution)
		{
			double value = 0;
			Action valueAnim([&](double t) { value = t * 2.0; });
			Before animation(1.0, valueAnim);
			value = 667.0; animation.PlaySimple(0.0); Assert::AreNotEqual(667.0, value, 0.01, L"Value should change.");
			value = 667.0; animation.PlaySimple(1.0); Assert::AreEqual(667.0, value, 0.01, L"Value should not change.");
			value = 667.0; animation.PlaySimple(2.0); Assert::AreEqual(667.0, value, 0.01, L"Value should change.");
		}

		TEST_METHOD(TestAnimationBetweenConditionalExecution)
		{
			double value = 0;
			Action valueAnim([&](double t) { value = t * 2.0; });
			Between animation(1.0, 2.0, valueAnim);
			value = 667.0; animation.PlaySimple(0.0); Assert::AreEqual(667.0, value, 0.01, L"Value not should change.");
			value = 667.0; animation.PlaySimple(0.5); Assert::AreEqual(667.0, value, 0.01, L"Value not should change.");
			value = 667.0; animation.PlaySimple(1.0); Assert::AreNotEqual(667.0, value, 0.01, L"Value should change.");
			value = 667.0; animation.PlaySimple(1.1); Assert::AreNotEqual(667.0, value, 0.01, L"Value should change.");
			value = 667.0; animation.PlaySimple(1.9); Assert::AreNotEqual(667.0, value, 0.01, L"Value should change.");
			value = 667.0; animation.PlaySimple(2.0); Assert::AreEqual(667.0, value, 0.01, L"Value not should change.");
			value = 667.0; animation.PlaySimple(2.5); Assert::AreEqual(667.0, value, 0.01, L"Value not should change.");
		}

		TEST_METHOD(TestAnimationAfterValue)
		{
			double value = 0;
			Action valueAnim([&](double t) { value = t * 2.0; });
			After animation(1.0, valueAnim);
			animation.PlaySimple(2.0); Assert::AreEqual(2.0, value, 0.01, L"Value should be 2.0");
			animation.PlaySimple(3.0); Assert::AreEqual(4.0, value, 0.01, L"Value should be 4.0");
			animation.PlaySimple(4.0); Assert::AreEqual(6.0, value, 0.01, L"Value should be 6.0");
		}

		TEST_METHOD(TestAnimationBeforeValue)
		{
			double value = 0;
			Action valueAnim([&](double t) { value = t * 2.0; });
			Before animation(1.0, valueAnim);
			animation.PlaySimple(+0.0); Assert::AreEqual(-2.0, value, 0.01, L"Value should be -2.0");
			animation.PlaySimple(-1.0); Assert::AreEqual(-4.0, value, 0.01, L"Value should be -4.0");
			animation.PlaySimple(-2.0); Assert::AreEqual(-6.0, value, 0.01, L"Value should be -6.0");
		}

		TEST_METHOD(TestAnimationBetweenValue)
		{
			double value = 0;
			Action valueAnim([&](double t) { value = t * 2.0; });
			Between animation(1.0, 4.0, valueAnim);
			animation.PlaySimple(1.0); Assert::AreEqual(0.0, value, 0.01, L"Value should be 0.0");
			animation.PlaySimple(2.0); Assert::AreEqual(2.0, value, 0.01, L"Value should be 2.0");
			animation.PlaySimple(3.0); Assert::AreEqual(4.0, value, 0.01, L"Value should be 4.0");
		}

		TEST_METHOD(TestAnimationSeekLogic)
		{
			double value = 0;
			Action action([&](double t) { value = t * 2.0; });
			Seek animation(1.0, action);
			animation.PlaySimple(1); Assert::AreEqual(4.0, value, 0.01, L"Value should be 4.0 ((t+1)*2)");
			animation.PlaySimple(2); Assert::AreEqual(6.0, value, 0.01, L"Value should be 6.0 ((t+1)*2)");
		}

		TEST_METHOD(TestAnimationEventLogic)
		{
			double value = 0;
			Action action([&](double t) { value = t * 2.0; });
			Event animation(1.0, action);
			double prev = 0.0;
			// forward event detect
			for (double i = 0.125; i < 2.0; i += 0.25)
			{
				value = 667.0;
				animation.Play(i, prev);
				if (i == 1.125)
				{
					Assert::AreNotEqual(667.0, value, 0.01, L"Value should change.");
				} 
				else
				{
					Assert::AreEqual(667.0, value, 0.01, L"Value not should change.");
				}
				prev = i;
			}
			// backwards event detect
			for (double i = 2.125; i >= 0.0; i -= 0.25)
			{
				value = 667.0;
				animation.Play(i, prev);
				if (i == 0.75+0.125)
				{
					Assert::AreNotEqual(667.0, value, 0.01, L"Value should change.");
				}
				else
				{
					Assert::AreEqual(667.0, value, 0.01, L"Value not should change.");
				}
				prev = i;
			}
		}

		TEST_METHOD(TestAnimationStretchLogic)
		{
			double value = 0;
			Action action([&](double t) { value = t * 2.0; });
			Stretch animation(0.5, action);
			animation.PlaySimple(1); Assert::AreEqual(1.0, value, 0.01, L"Value should be 1.0 ((t*.5)*2)");
			animation.PlaySimple(2); Assert::AreEqual(2.0, value, 0.01, L"Value should be 2.0 ((t*.5)*2)");
		}

		TEST_METHOD(TestAnimationTimeTransformLogic)
		{
			double value = 0;
			Action action([&](double t) { value = t * 2.0; });
			TimeTransform animation([](double t) { return t*.5 + 2.0; }, action);
			animation.PlaySimple(2); Assert::AreEqual(6.0, value, 0.01, L"Value should be 6.0 ((t*.5+2.0)*2)");
			animation.PlaySimple(4); Assert::AreEqual(8.0, value, 0.01, L"Value should be 8.0 ((t*.5+2.0)*2)");
		}

		TEST_METHOD(TestAnimationParallelConstructor)
		{
			double x = 0, y = 1;
			Action action1([&](double t) { x = t * 2.0; });
			Action action2([&](double t) { y = t * 3.0; });
			Parallel animation(action1, action2);
			animation.PlaySimple(2.0); 
			Assert::AreEqual(4.0, x, 0.01);
			Assert::AreEqual(6.0, y, 0.01);
			animation.PlaySimple(4.0);
			Assert::AreEqual(8.0, x, 0.01);
			Assert::AreEqual(12.0, y, 0.01);
		}

		TEST_METHOD(TestAnimationParallelAppend)
		{
			double x = 0, y = 1;
			Action action1([&](double t) { x = t * 2.0; });
			Action action2([&](double t) { y = t * 3.0; });
			Parallel animation;
			animation.Add(action1);
			animation.PlaySimple(2.0);
			Assert::AreEqual(4.0, x, 0.01);
			animation.Add(action2);
			animation.PlaySimple(4.0);
			Assert::AreEqual(8.0, x, 0.01);
			Assert::AreEqual(12.0, y, 0.01);
		}

	};
}