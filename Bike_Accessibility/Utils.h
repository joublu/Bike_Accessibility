#pragma once

#ifndef UTILS_H
#define UTILS_H


#include "Node.h"
#include "PCC.h"

namespace myUtils
{


	inline bool sortbydecreasdist(const Node* a,	const Node* b)
	{
		return (a->getDist() > b->getDist()  );
	}

	inline bool sortbydecreasdistPCC(const PCC* a, const PCC* b)
	{
		return (a->getDist() > b->getDist()  );
	}

}

#endif