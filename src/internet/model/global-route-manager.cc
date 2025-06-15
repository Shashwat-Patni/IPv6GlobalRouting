/*
 * Copyright 2007 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Henderson (tomhend@u.washington.edu)
 */

#include "global-route-manager.h"

#include "global-route-manager-impl.h"

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/simulation-singleton.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("GlobalRouteManager");

// ---------------------------------------------------------------------------
//
// GlobalRouteManager Implementation
//
// ---------------------------------------------------------------------------

template <typename T, typename Enable>
void
GlobalRouteManager<T, Enable>::DeleteGlobalRoutes()
{
    NS_LOG_FUNCTION_NOARGS();
    SimulationSingleton<GlobalRouteManagerImpl>::Get()->DeleteGlobalRoutes();
}

template <typename T, typename Enable>
void
GlobalRouteManager<T, Enable>::BuildGlobalRoutingDatabase()
{
    NS_LOG_FUNCTION_NOARGS();
    SimulationSingleton<GlobalRouteManagerImpl>::Get()->BuildGlobalRoutingDatabase();
}

template <typename T, typename Enable>
void
GlobalRouteManager<T, Enable>::InitializeRoutes()
{
    NS_LOG_FUNCTION_NOARGS();
    SimulationSingleton<GlobalRouteManagerImpl>::Get()->InitializeRoutes();
}

template <typename T, typename Enable>
uint32_t
GlobalRouteManager<T, Enable>::AllocateRouterId()
{
    NS_LOG_FUNCTION_NOARGS();
    return routerId++;
}

template <typename T, typename Enable>
void
GlobalRouteManager<T, Enable>::ResetRouterId()
{
    routerId = 0;
}

template <typename T, typename Enable>
void
GlobalRouteManager<T, Enable>::PrintRoutingPath(Ptr<Node> sourceNode,
                                                IpAddress dest,
                                                Ptr<OutputStreamWrapper> stream,
                                                Time::Unit unit)
{
    SimulationSingleton<GlobalRouteManagerImpl>::Get()->PrintRoutingPath(sourceNode,
                                                                         dest,
                                                                         stream,
                                                                         unit);
}

template <typename T, typename Enable>
uint32_t GlobalRouteManager<T, Enable>::routerId = 0;

template class ns3::GlobalRouteManager<ns3::Ipv4Manager>;

} // namespace ns3
