#pragma once

#include <iostream>
#include <chrono>

class Helper {
public:
	template <typename T>
	static void Say(const T& output) {
		std::cout << output << std::endl;
	}
	Helper() : start_time_(std::chrono::steady_clock::now()) {}
	bool hasMoreThanOneSecondPassed(double duration) {
		auto current_time = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = current_time - start_time_;
		if (elapsed.count() > duration) {
			start_time_ = current_time;
			return true;
		}
		return false;
	}

private:
	std::chrono::steady_clock::time_point start_time_;
};
