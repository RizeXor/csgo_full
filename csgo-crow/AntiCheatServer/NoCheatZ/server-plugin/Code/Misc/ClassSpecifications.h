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

#ifndef CLASSSPECIFICATIONS_H
#define CLASSSPECIFICATIONS_H

class NoCopy
{
protected:
	NoCopy ()
	{}
	~NoCopy ()
	{}

private:
	NoCopy ( NoCopy const & ) = delete;
	NoCopy& operator=( NoCopy const & ) = delete;
};

class NoMove
{
protected:
	NoMove ()
	{}
	~NoMove ()
	{}

private:
	NoMove ( NoMove && ) = delete;
	NoMove& operator=( NoMove && ) = delete;
};

#endif
