//
//  debugprintf.h
//  Tree exercises
//
//  Created by Eric on 2/27/14.
//  Copyright (c) 2014 erflink. All rights reserved.
//

#ifndef Tree_exercises_debugprintf_h
#define Tree_exercises_debugprintf_h
#define DEBUG_OUTPUT

#ifdef DEBUG_OUTPUT
#define debugPrintf(x) printf(x)
#define debugPrintf1(x, y) printf(x, y)
#define debugPrintf2(x, y, z) printf(x, y, z)
#define debugPrintf3(x, y, z, xx) printf(x, y, z, xx)
#else
#define debugPrintf(x)
#define debugPrintf1(x, y)
#define debugPrintf2(x, y, z)
#define debugPrintf3(x, y, z, xx)
#endif

#endif
