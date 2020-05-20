#pragma once

class GamePlayCallBack
{
public:
	GamePlayCallBack();
	~GamePlayCallBack();


	virtual void update(const float currentTime, const float elapsedTime) = 0;

};