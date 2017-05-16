// AnimatAnything in C++
// a library for scheduling, executing and programming sequences, events and so on

#pragma once

#include <functional>
#include <vector>

namespace AnimateAnything
{
	// Base interface for animations, you may create your own derived types if you wish
	template<typename NumericType> class IAnimation
	{
	public:
		virtual void Play(NumericType t, NumericType t0) = 0; // Play the animation at moment t, previous moment given in t0, (deltatime or event detect)
		virtual void PlaySimple(NumericType t) { Play(t, t); } // you should call 2 argument version instaead, this is added as helper
		virtual ~IAnimation() { }; // polymorphic class
	};

	// Animation that contains a lambda with no parameters
	template<typename NumericType> class AnimationActionVoid : public IAnimation<NumericType>
	{
	public:
		std::function<void(void)> Action;
		void Play(NumericType t, NumericType t0) override { Action(); }
		AnimationActionVoid(std::function<void(void)> action) : Action(action) { };
		~AnimationActionVoid() { };
	};

	// Animation that contains a lambda with time
	template<typename NumericType> class AnimationActionTime : public IAnimation<NumericType>
	{
	public:
		std::function<void(NumericType)> Action;
		void Play(NumericType t, NumericType t0) override { Action(t); }
		AnimationActionTime(std::function<void(NumericType)> action) : Action(action) { };
		~AnimationActionTime() { };
	};

	// Animation that happens after a specified moment
	template<typename NumericType> class AnimationAfter : public IAnimation<NumericType>
	{
	public:
		NumericType Start;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { if (Start <= t) Animation.Play(t - Start, t0 - Start); }
		AnimationAfter(NumericType start, IAnimation<NumericType>& action) : Start(start), Animation(action) { }
		~AnimationAfter(){ }
	};

	// Animation that happens before a specified moment
	template<typename NumericType> class AnimationBefore : public IAnimation<NumericType>
	{
	public:
		NumericType Finish;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { if (t < Finish) Animation.Play(t - Finish, t0 - Finish); }
		AnimationBefore(NumericType finish, IAnimation<NumericType>& action) : Finish(finish), Animation(action) { }
		~AnimationBefore() { }
	};

	// Animation that happens between two moments
	template<typename NumericType> class AnimationBetween : public IAnimation<NumericType>
	{
	public:
		NumericType Start;
		NumericType Finish;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { if (Start <= t && t < Finish) Animation.Play(t - Start, t0 - Start); }
		AnimationBetween(NumericType start, NumericType finish, IAnimation<NumericType>& action) : Start(start), Finish(finish), Animation(action) { }
		AnimationBetween(NumericType start, NumericType finish, IAnimation<NumericType>* action) : Start(start), Finish(finish), Animation(*action) { }
		~AnimationBetween() { }
	};

	// Skip part of the animation
	template<typename NumericType> class AnimationSeek : public IAnimation<NumericType>
	{
	public:
		NumericType Skip;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { Animation.Play(t + Skip, t0 + Skip); }
		AnimationSeek(NumericType skip, IAnimation<NumericType>& action) : Skip(skip), Animation(action) { }
		~AnimationSeek() { }
	};

	// An event is only triggered once, detection is performed based on t and t0
	template<typename NumericType> class AnimationEvent : public IAnimation<NumericType>
	{
	public:
		NumericType Moment;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { if (Moment <= t && t0 < Moment || t <= Moment && Moment < t0) Animation.Play(t, t0); }
		AnimationEvent(NumericType start, IAnimation<NumericType>& action) : Moment(start), Animation(action) { }
		~AnimationEvent() { }
	};

	// Scale the timeunit by a constant
	template<typename NumericType> class AnimationStretch : public IAnimation<NumericType>
	{
	public:
		NumericType Scale;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { Animation.Play(t*Scale, t0*Scale); }
		AnimationStretch(NumericType scale, IAnimation<NumericType>& action) : Scale(scale), Animation(action) { }
		~AnimationStretch() { }
	};

	// Alter timeline using custom lambda function
	template<typename NumericType> class AnimationTimeTransform : public IAnimation<NumericType>
	{
	public:
		std::function<NumericType(NumericType)> Transform;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { Animation.Play(Transform(t), Transform(t0)); }
		AnimationTimeTransform(std::function<NumericType(NumericType)> transform, IAnimation<NumericType>& action) : Transform(transform), Animation(action) { }
		~AnimationTimeTransform() { }
	};

	// Run multiple animations
	template<typename NumericType> class AnimationParallel : public IAnimation<NumericType>
	{
	public:
		std::vector<IAnimation<NumericType>*> Animations;
		void Play(NumericType t, NumericType t0) override { for (auto& animation : Animations) animation->Play(t, t0); }
		AnimationParallel() { }

		template<typename H> AnimationParallel(H& item)
		{
			Animations.push_back(&item);
		}

		template<typename H, typename... T> AnimationParallel(H& item, T&... rest) :
			AnimationParallel(rest...)
		{
			Animations.push_back(&item);
		}

		template<typename H> AnimationParallel(H* item)
		{
			Animations.push_back(item);
		}

		template<typename H, typename... T> AnimationParallel(H* item, T*... rest) :
			AnimationParallel(rest...)
		{
			Animations.push_back(item);
		}

		void Add(IAnimation<NumericType>& item)
		{
			Animations.push_back(&item);
		}

		void Add(IAnimation<NumericType>* item)
		{
			Animations.push_back(item);
		}

		~AnimationParallel() { }
	};

	// TODO loop

	// Container class builds animations, stores animations and manages memory so that you don't have to
	template<typename NumericType> class Container
	{
	private:

		// a vector containing all the animations in this container
		std::vector<IAnimation<NumericType>*> ownedAnimations;

		// make an animation node and convert it to stuff
		template<typename Type, typename ...Args> Type* MakeNode(Args... args)
		{
			Type* node = new Type(args...);
			ownedAnimations.push_back(node);
			return node;
		}

		// Append  - general case
		template<typename H, typename ...T> void Append(AnimationParallel<NumericType>* target, H first, T... rest)
		{
			target->Add(Parallel(first));
			Append(target, rest...);
		}

		// Append - stop condition
		template<typename H, typename ...T> void Append(AnimationParallel<NumericType>* target, H first)
		{
			target->Add(Parallel(first));
			// no more recursive call
		}

	public:
		
		// Parallel with 1 parameter, degenrate case, only one IAnimation node, return node
		IAnimation<NumericType>* Parallel(IAnimation<NumericType>* node)
		{
			return node;
		}

		// Parallel with 1 parameter, degenerate case, only one lambda
		IAnimation<NumericType>* Parallel(std::function<void(NumericType)> action)
		{
			return MakeNode<AnimationActionTime<NumericType>>(action);
		}

		// Parallel with 1 parameter, degenerate case, only one lambda without param
		IAnimation<NumericType>* Parallel(std::function<void(void)> action)
		{
			return MakeNode<AnimationActionVoid<NumericType>>(action);
		}

		// Parallel with 2 or more prameters
		template<typename H1, typename H2, typename ...Tail> IAnimation<NumericType>* Parallel(H1 first, H2 second, Tail... rest)
		{
			auto node = MakeNode<AnimationParallel<NumericType>>();
			Append(node, first, second, rest...);
			return node;
		}

		// Animation between 2 points in time
		template<typename ...Args> IAnimation<NumericType>* Between(NumericType start, NumericType finish, Args... args)
		{
			return MakeNode<AnimationBetween<NumericType>>(start, finish, Parallel(args...));
		}

		// Animation before a specific point in time
		template<typename ...Args> IAnimation<NumericType>* Before(NumericType moment, NumericType finish, Args... args)
		{
			return MakeNode<AnimationBefore<NumericType>>(moment, Parallel(args...));
		}

		// Animation before a specific point in time
		template<typename ...Args> IAnimation<NumericType>* After(NumericType moment, NumericType finish, Args... args)
		{
			return MakeNode<AnimationAfter<NumericType>>(moment, Parallel(args...));
		}

		// Animation before a specific point in time
		template<typename ...Args> IAnimation<NumericType>* Event(NumericType moment, NumericType finish, Args... args)
		{
			return MakeNode<AnimationEvent<NumericType>>(moment, Parallel(args...));
		}

		// Stretch
		template<typename ...Args> IAnimation<NumericType>* Stretch(NumericType amount, NumericType finish, Args... args)
		{
			return MakeNode<AnimationStretch<NumericType>>(amount, Parallel(args...));
		}

		// Skip part of animation
		template<typename ...Args> IAnimation<NumericType>* Seek(NumericType amount, NumericType finish, Args... args)
		{
			return MakeNode<AnimationSeek<NumericType>>(amount, Parallel(args...));
		}

		// Custom time transform for an animation
		template<typename ...Args> IAnimation<NumericType>* TimeTransform(std::function<NumericType(NumericType)> transform, NumericType finish, Args... args)
		{
			return MakeNode<AnimationTimeTransform<NumericType>>(transform, Parallel(args...));
		}

		~Container()
		{
			for(auto node : ownedAnimations)
			{
				delete node;
			}
		}
	};
}
