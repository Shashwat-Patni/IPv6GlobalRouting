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

template <typename T>
uint32_t GlobalRouteManager<T>::routerId = 0; //!< Router ID counter

template <typename T>
void
GlobalRouteManager<T>::DeleteGlobalRoutes()
{
    NS_LOG_FUNCTION_NOARGS();
    SimulationSingleton<
        GlobalRouteManagerImpl<typename GlobalRouteManager<T, Enable>::IpManager>>::Get()
        ->DeleteGlobalRoutes();
}

template <typename T>
void
GlobalRouteManager<T>::BuildGlobalRoutingDatabase()
{
    NS_LOG_FUNCTION_NOARGS();
    SimulationSingleton<
        GlobalRouteManagerImpl<typename GlobalRouteManager<T, Enable>::IpManager>>::Get()
        ->BuildGlobalRoutingDatabase();
}

template <typename T>
void
GlobalRouteManager<T>::InitializeRoutes()
{
    NS_LOG_FUNCTION_NOARGS();
    SimulationSingleton<
        GlobalRouteManagerImpl<typename GlobalRouteManager<T, Enable>::IpManager>>::Get()
        ->InitializeRoutes();
}

template <typename T>
uint32_t
GlobalRouteManager<T>::AllocateRouterId()
{
    NS_LOG_FUNCTION_NOARGS();
    return routerId++;
}

template <typename T>
void
GlobalRouteManager<T>::ResetRouterId()
{
    routerId = 0;
}

template <typename T>
void
GlobalRouteManager<T>::PrintRoutingPath(Ptr<Node> sourceNode,
                                        IpAddress dest,
                                        Ptr<OutputStreamWrapper> stream,
                                        Time::Unit unit)
{
    SimulationSingleton<
        GlobalRouteManagerImpl<typename GlobalRouteManager<T, Enable>::IpManager>>::Get()
        ->PrintRoutingPath(sourceNode, dest, stream, unit);
}

template class ns3::GlobalRouteManager<ns3::Ipv4Manager>;

} // namespace ns3
