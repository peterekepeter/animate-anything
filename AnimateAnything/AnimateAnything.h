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

	// Animation that contains a lambda
	template<typename NumericType> class AnimationAction : public IAnimation<NumericType>
	{
	public:
		std::function<void(NumericType)> Action;
		void Play(NumericType t, NumericType t0) override { Action(t); }
		AnimationAction(std::function<void(NumericType)> action) : Action(action) { };
		~AnimationAction() { };
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
		~AnimationBetween() { }
	};

	// Skip part of the animation
	template<typename NumericType> class AnimationOffset : public IAnimation<NumericType>
	{
	public:
		NumericType Skip;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { Animation.Play(t + Skip, t0 + Skip); }
		AnimationOffset(NumericType skip, IAnimation<NumericType>& action) : Skip(skip), Animation(action) { }
		~AnimationOffset() { }
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
	template<typename NumericType> class AnimationTimestretch : public IAnimation<NumericType>
	{
	public:
		NumericType Scale;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { Animation.Play(t*Scale, t0*Scale); }
		AnimationTimestretch(NumericType scale, IAnimation<NumericType>& action) : Scale(scale), Animation(action) { }
		~AnimationTimestretch() { }
	};

	// Alter timeline using custom lambda function
	template<typename NumericType> class AnimationTransform : public IAnimation<NumericType>
	{
	public:
		std::function<NumericType(NumericType)> Transform;
		IAnimation<NumericType>& Animation;
		void Play(NumericType t, NumericType t0) override { Animation.Play(Transform(t), Transform(t0)); }
		AnimationTransform(std::function<NumericType(NumericType)> transform, IAnimation<NumericType>& action) : Transform(transform), Animation(action) { }
		~AnimationTransform() { }
	};

	// Run multiple animations
	template<typename NumericType> class AnimationComposite : public IAnimation<NumericType>
	{
	public:
		std::vector<IAnimation<NumericType>*> Animations;
		void Play(NumericType t, NumericType t0) override { for (auto& animation : Animations) animation->Play(t, t0); }
		AnimationComposite() { }

		template<typename H> AnimationComposite(H& item)
		{
			Animations.push_back(&item);
		}

		template<typename H, typename... T> AnimationComposite(H& item, T&... rest) :
			AnimationComposite(rest...)
		{
			Animations.push_back(&item);
		}

		void Add(IAnimation<NumericType>& item)
		{
			Animations.push_back(&item);
		}

		~AnimationComposite() { }
	};

	// TODO loop

	// Builder / Container Class
	template<typename NumericType> class AnimateAnything
	{
		// TODO
	};
}
