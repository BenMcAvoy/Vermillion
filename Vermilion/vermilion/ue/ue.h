#pragma once

#include "math.h"
#include "containers.h"
#include "strings.h"

#include "uobject.h"

#include "uworld.h"

namespace Vermilion::UE {
	void Init() {
		FNamePool::Get();
		UWorld::Get();
	}
}
