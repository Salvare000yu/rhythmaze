#pragma once
class Looper {
private:
	Looper(const Looper& looper) = delete;
	Looper& operator=(const Looper& looper) = delete;

	Looper();
	~Looper();

public:
	static Looper* getInstance();


	// @return 異常の有無(falseで正常)
	bool loop();
};

