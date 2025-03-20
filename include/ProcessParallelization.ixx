export module ProcessParallelization;
import <iostream>;
import <concepts>;
import <array>;
import <vector>;
import <list>;
import <thread>;
import <functional>;
import <ranges>;
import <algorithm>;
import <any>;
import <deque>;
import <future>;
import <queue>;
import <memory>;
import <condition_variable>;

template <typename Type>
concept Arrayable = requires(Type t)
{
	std::begin(t);
	std::end(t);
	std::size(t);
};

template <Arrayable T>
void HelperThread(const T& array, const std::function<void(typename T::value_type)>& func, size_t beg, size_t end)
{
	for (auto i{ beg }; i < end; ++i)
		func(array[i]);
}

export
{
	template <typename Type>
	class ThreadSafeDeque
	{
	public:
		ThreadSafeDeque() = default;

		void push_front(Type&& value)
		{
			std::unique_lock lock(m_mutex);
			m_deque.push_front(std::forward<Type&&>(value));
			m_waitCondition.notify_one();
		}

		void push_back(Type&& value)
		{
		    std::unique_lock lock(m_mutex);
			m_deque.push_back(std::forward<Type&&>(value));
			m_waitCondition.notify_one();
		}

		bool pop_front(Type& value)
		{
			std::unique_lock lock(m_mutex);
			if (m_deque.empty())
				return false;
			value = std::move(m_deque.front());
			m_deque.pop_front();
			return true;
		}

		bool pop_back(Type& value)
		{
			std::unique_lock lock(m_mutex);
			if (m_deque.empty())
				return false;
			value = std::move(m_deque.back());
			m_deque.pop_back();
			return true;
		}

		void WaitNotEmpty()
		{
            std::unique_lock lock(m_mutex);
            m_waitCondition.wait(lock, [this] { return !m_deque.empty() || m_terminate; });
		}

		void TerminateAll()
		{
			std::unique_lock lock(m_mutex);
            m_terminate = true;
			m_deque.clear();
			m_waitCondition.notify_all();
		}

	private:
        std::atomic_bool m_terminate{ false };
		std::deque<Type> m_deque;
		std::mutex m_mutex;
		std::condition_variable m_waitCondition;
	};

	template <Arrayable T>
	auto Parallel_For(const T& array, const std::function<void(typename T::value_type)>& func)
	{
		const auto MaxThreads = std::thread::hardware_concurrency();
		const auto elementsPerThread = std::size(array) / MaxThreads;
		std::vector<std::jthread> threads;
		threads.reserve(MaxThreads);
		for (size_t i{ 0 }; i < MaxThreads; ++i)
			threads.emplace_back(HelperThread<T>, std::cref(array), func, i * elementsPerThread, (i != MaxThreads - 1) ? (i + 1) * elementsPerThread : std::size(array));

		std::ranges::for_each(threads, std::mem_fn(&std::jthread::join));
	}

	class ThreadPool
	{
	public:
		ThreadPool();
		~ThreadPool()
		{
			running = false;
			workQueue.TerminateAll();
			std::ranges::for_each(threads, std::mem_fn(&std::jthread::join));
		};

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;

		std::future<std::any> RegisterWork(const std::function<std::any()>& func);
	private:
		std::vector<std::jthread> threads;
		ThreadSafeDeque<std::packaged_task<std::any()>> workQueue;
		void WorkerThread();
		std::atomic_bool running{ true };
	};

}


std::future<std::any> ThreadPool::RegisterWork(const std::function<std::any()>& func)
{
	std::packaged_task<std::any()> task(func);
	std::future<std::any> future(task.get_future());
	workQueue.push_back(std::move(task));
	return std::move(future);
}


ThreadPool::ThreadPool()
{
	const auto MaxThreads = std::thread::hardware_concurrency();
	for (auto i{ 0ul }; i < MaxThreads; ++i)
	{
		threads.emplace_back(&ThreadPool::WorkerThread, this);
	}
}

void ThreadPool::WorkerThread()
{
	while (running)
	{
		if (std::packaged_task<std::any()> task; workQueue.pop_front(task))
		{
			task();
		}
		else
		{
            workQueue.WaitNotEmpty();
		}
	}
}
