/* 
   Copyright 2015 Quazar Technologies Pvt. Ltd.
   Copyright 2015 Chintalagiri Shashank
   
   This file is part of
   Embedded bootstraps : Subsystem Interfaces
   
   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

/**
 * @file sys.h
 * @brief Highest level include for system libraries.
 *
 * This file acts as a proxy for specific subsystem include headers. Project 
 * firmware could include this file instead of the subsystem specific 
 * headers, or it could include only the needed subsystem headers.
 * 
 * As such, this file should not have any major functional code elements. 
 * Various definitions needed for configuring the lower level modules 
 * should be placed in and used from `bsp/board.h` and `bsp/uc_pum.h` and 
 * it's include chain.
 * 
 * In general, the subsystem libraries should be thought of as a translation
 * layer between application code, the underlying hardware drivers, and any
 * external libraries the subsystem depends on. In cases where a subsystem 
 * depends on external libraries, this layer along with the HAL should provide 
 * enough hooks to port the usage of library to other applications and 
 * platforms. 
 * 
 * As of now, the subsystem layer handles :
 *      
 *   - Translation of subsystem functions into HAL functions, and the 
 *     associated routing of interfaces. Switching a subsystem from one 
 *     interface to another is implemented here.
 *   - Providing port-specific handlers for external library based 
 *     subsystems. All functions the library expects to see in the 
 *     application with hardware-specific implementations are defined
 *     here and reroute to the correct interface's HAL.  
 *
 */

#ifndef SUBSYSTEMS_H
#define SUBSYSTEMS_H

#include "bciface.h"

#endif
