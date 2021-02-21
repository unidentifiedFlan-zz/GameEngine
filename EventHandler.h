#pragma once
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <functional>
#include <iostream>

template<class T>
class EventHandler
{
private:
	EventHandler<T>() 
	{}

public:
	static EventHandler<T>* instance()
	{
		if (_instance == nullptr)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			if (_instance == nullptr)
			{
				_instance = new EventHandler<T>();
			}
		}
		return _instance;
	}

	template< class F, class G >
	void addListener(F&& callback, G *object)
	{
		intptr_t hash = reinterpret_cast <intptr_t> (&callback) + reinterpret_cast <intptr_t> (object);
		callbacks[hash] = std::bind(std::forward<F>(callback), object, std::placeholders::_1);
	}

	template< class F, class G >
	void removeListener(F&& callback, G *object)
	{
		intptr_t hash = reinterpret_cast <intptr_t> (&callback) + reinterpret_cast <intptr_t> (object);
		callbacks.erase(hash);
	}

	void raise(const T& evnt)
	{
		for (auto const& x : callbacks)
		{
			try
			{
				x.second(evnt);
			}
			catch (std::exception e)
			{
				std::cout << "Error raising event. Possible null function ptr" << std::endl;
			}
		}
	}

private:
	static std::atomic<EventHandler<T>*> _instance;
	static std::mutex _mutex;
	std::unordered_map<intptr_t, std::function<void(const T&)>> callbacks;
};

template <class T>
std::atomic<EventHandler<T>*> EventHandler<T>::_instance{ nullptr };
template <class T>
std::mutex EventHandler<T>::_mutex;