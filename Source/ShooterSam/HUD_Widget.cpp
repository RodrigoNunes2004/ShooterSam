// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_Widget.h"

void UHUD_Widget::SetHealthBarPercent(float NewPercent)
{
	if (NewPercent >= 0.0f && NewPercent <= 1.0f)
	{
		HealthBar->SetPercent(NewPercent);
	}
}
