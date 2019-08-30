/*
	Copyright 2012 - Le Padellec Sylvain

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdio.h>
#include <cmath>

#include "EyeAnglesTester.h"

#include "Misc/EntityProps.h"
#include "Systems/BanRequest.h"

EyeAnglesTester::EyeAnglesTester(void) : BaseTesterSystem("EyeAnglesTester"),
										 playerdata_class(),
										 PlayerRunCommandHookListener(),
										 UserMessageHookListener(),
										 Singleton()
{
}

EyeAnglesTester::~EyeAnglesTester(void)
{
	Unload();
}

void EyeAnglesTester::Init()
{
	InitDataStruct();
}

void EyeAnglesTester::Load()
{
	for (PlayerHandler::iterator it(PlayerHandler::begin()); it != PlayerHandler::end(); ++it)
	{
		ResetPlayerDataStructByIndex(it.GetIndex());
	}

	PlayerRunCommandHookListener::RegisterPlayerRunCommandHookListener(this, SystemPriority::EyeAnglesTester);
	UserMessageHookListener::RegisterUserMessageHookListener(this);
}

void EyeAnglesTester::Unload()
{
	PlayerRunCommandHookListener::RemovePlayerRunCommandHookListener(this);
	UserMessageHookListener::RemoveUserMessageHookListener(this);
}

bool EyeAnglesTester::GotJob() const
{
	// Create a filter
	ProcessFilter::HumanAtLeastConnected const filter_class;
	// Initiate the iterator at the first match in the filter
	PlayerHandler::iterator it(&filter_class);
	// Return if we have job to do or not ...
	return it != PlayerHandler::end();
}

PlayerRunCommandRet EyeAnglesTester::RT_PlayerRunCommandCallback(PlayerHandler::iterator ph, void *const pCmd, double const &curtime)
{
	int const *const flags(g_EntityProps.GetPropValue<int, PROP_FLAGS>(ph->GetEdict()));
	EyeAngleInfoT *playerData(GetPlayerDataStructByIndex(ph.GetIndex()));
	playerData->x.abs_value = fabs(playerData->x.value = static_cast<SourceSdk::CUserCmd_csgo *>(pCmd)->viewangles.x);
	playerData->y.abs_value = fabs(playerData->y.value = static_cast<SourceSdk::CUserCmd_csgo *>(pCmd)->viewangles.y);
	playerData->z.abs_value = fabs(playerData->z.value = static_cast<SourceSdk::CUserCmd_csgo *>(pCmd)->viewangles.z);

	/*
		FL_FROZEN			(1 << 5)
		FL_ATCONTROLS		(1 << 6)
	*/

	if ((*flags & (3 << 5)) == 0)
	{
		if (playerData->x.abs_value > 89.0f && playerData->past_x.abs_value > 89.0f && playerData->x.value != playerData->past_x.value)
		{
			++playerData->x.detectionsCount;
			//drop_cmd = PlayerRunCommandRet::INERT;
			if (playerData->x.lastDetectionPrintTime + ANTIFLOOD_LOGGING_TIME < curtime)
			{
				playerData->x.lastDetectionPrintTime = curtime;

				if (playerData->x.detectionsCount > 5)
				{
					ProcessDetectionAndTakeAction<Detection_EyeAngleX::data_type>(Detection_EyeAngleX(), playerData, ph, this);
				}
			}
		}
		if (playerData->y.abs_value > 180.0f && playerData->past_y.abs_value > 180.0f && playerData->y.value != playerData->past_y.value)
		{
			++playerData->y.detectionsCount;
			//drop_cmd = PlayerRunCommandRet::INERT;
			if (playerData->y.lastDetectionPrintTime + ANTIFLOOD_LOGGING_TIME < curtime)
			{
				playerData->y.lastDetectionPrintTime = curtime;

				if (playerData->y.detectionsCount > 5)
				{
					ProcessDetectionAndTakeAction<Detection_EyeAngleY::data_type>(Detection_EyeAngleY(), playerData, ph, this);
				}
			}
		}
		if (playerData->z.abs_value > 0.5f && playerData->past_z.abs_value > 0.5f && playerData->z.value != playerData->past_z.value)
		{
			if (!Helpers::IsInt(playerData->z.value)) // Don't detect ""fun"" plugins
			{
				++playerData->z.detectionsCount;
				//drop_cmd = PlayerRunCommandRet::INERT;
				if (playerData->z.lastDetectionPrintTime + ANTIFLOOD_LOGGING_TIME < curtime)
				{
					playerData->z.lastDetectionPrintTime = curtime;

					if (playerData->z.detectionsCount > 5)
					{
						ProcessDetectionAndTakeAction<Detection_EyeAngleZ::data_type>(Detection_EyeAngleZ(), playerData, ph, this);
					}
				}
			}
		}
	}

	playerData->past_x = playerData->x;
	playerData->past_y = playerData->y;
	playerData->past_z = playerData->z;

	return PlayerRunCommandRet::CONTINUE;
}

bool EyeAnglesTester::RT_SendUserMessageCallback(SourceSdk::IRecipientFilter const &filter, int const message_id, google::protobuf::Message const &buffer)
{
	DebugMessage(Helpers::format("EyeAnglesTester::RT_SendUserMessageCallback : %d, %s", message_id, buffer.DebugString().c_str()));

	return false;
}

bool EyeAnglesTester::RT_UserMessageBeginCallback(SourceSdk::IRecipientFilter const *const filter, int const message_id)
{
	return false;
}

void EyeAnglesTester::RT_MessageEndCallback(SourceSdk::IRecipientFilter const *const filter, int const message_id, SourceSdk::bf_write *buffer)
{
}

EyeAnglesTester g_EyeAnglesTester;

basic_string Detection_EyeAngle::GetDataDump()
{
	return Helpers::format(
		":::: EyeAngleInfo {\n"
		":::::::: EyeAngleX {\n"
		":::::::::::: Angle : %f,\n"
		":::::::::::: Previous Angle : %f,\n"
		":::::::::::: Detections Count : %u\n"
		":::::::: },"
		"\n:::::::: EyeAngleY {\n"
		":::::::::::: Angle : %f,\n"
		":::::::::::: Previous Angle : %f,\n"
		":::::::::::: Detections Count : %u\n"
		":::::::: },\n"
		":::::::: EyeAngleZ {\n"
		":::::::::::: Angle : %f,\n"
		":::::::::::: Previous Angle : %f,\n"
		":::::::::::: Detections Count : %u\n"
		":::::::: }\n"
		":::: }",
		GetDataStruct()->x.value,
		GetDataStruct()->past_x.value,
		GetDataStruct()->x.detectionsCount,
		GetDataStruct()->y.value,
		GetDataStruct()->past_y.value,
		GetDataStruct()->y.detectionsCount,
		GetDataStruct()->z.value,
		GetDataStruct()->past_z.value,
		GetDataStruct()->z.detectionsCount);
}

basic_string Detection_EyeAngleX::GetDetectionLogMessage()
{
	if (Helpers::IsInt(GetDataStruct()->x.value))
	{
		return "Anti-Aim";
	}
	else
	{
		return "No recoil";
	}
}

basic_string Detection_EyeAngleY::GetDetectionLogMessage()
{
	if (Helpers::IsInt(GetDataStruct()->y.value))
	{
		return "Anti-Aim";
	}
	else
	{
		return "No recoil";
	}
}

basic_string Detection_EyeAngleZ::GetDetectionLogMessage()
{
	if (Helpers::IsInt(GetDataStruct()->z.value))
	{
		return "Anti-Aim";
	}
	else
	{
		return "No recoil";
	}
}
