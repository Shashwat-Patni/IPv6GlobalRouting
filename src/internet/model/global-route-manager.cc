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

#include <iomanip>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("GlobalRouteManager");

// ---------------------------------------------------------------------------
//
// GlobalRouteManager Implementation
//
// ---------------------------------------------------------------------------

void
GlobalRouteManager::DeleteGlobalRoutes()
{
    NS_LOG_FUNCTION_NOARGS();
    SimulationSingleton<GlobalRouteManagerImpl>::Get()->DeleteGlobalRoutes();
}

void
GlobalRouteManager::BuildGlobalRoutingDatabase()
{
    NS_LOG_FUNCTION_NOARGS();
    SimulationSingleton<GlobalRouteManagerImpl>::Get()->BuildGlobalRoutingDatabase();
}

void
GlobalRouteManager::InitializeRoutes()
{
    NS_LOG_FUNCTION_NOARGS();
    SimulationSingleton<GlobalRouteManagerImpl>::Get()->InitializeRoutes();
}

uint32_t
GlobalRouteManager::AllocateRouterId()
{
    NS_LOG_FUNCTION_NOARGS();
    static uint32_t routerId = 0;
    return routerId++;
}

void
GlobalRouteManager::PrintRoute(Ptr<Node> sourceNode,
                               Ipv4Address dest,
                               Ptr<OutputStreamWrapper> stream,
                               bool nodeIdLookup,
                               Time::Unit unit)
{
    std::ostream* os = stream->GetStream();
    // Copy the current ostream state
    std::ios oldState(nullptr);
    oldState.copyfmt(*os);

    *os << std::resetiosflags(std::ios::adjustfield) << std::setiosflags(std::ios::left);
    *os << "PrintRoute at Time: " << Now().As(unit);
    *os << " from Node " << sourceNode->GetId() << " to address " << dest;
    SimulationSingleton<GlobalRouteManagerImpl>::Get()->PrintRoute(sourceNode,
                                                                   dest,
                                                                   stream,
                                                                   nodeIdLookup,
                                                                   unit);
    (*os).copyfmt(oldState);
}

void
GlobalRouteManager::PrintRoute(Ptr<Node> sourceNode,
                               Ipv4Address dest,
                               bool nodeIdLookup,
                               Time::Unit unit)
{
    Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper>(&std::cout);
    GlobalRouteManager::PrintRoute(sourceNode, dest, stream, nodeIdLookup, unit);
}

void
GlobalRouteManager::PrintRoute(Ptr<Node> sourceNode,
                               Ptr<Node> dest,
                               Ptr<OutputStreamWrapper> stream,
                               bool nodeIdLookup,
                               Time::Unit unit)
{
    std::ostream* os = stream->GetStream();
    // Copy the current ostream state
    std::ios oldState(nullptr);
    oldState.copyfmt(*os);

    *os << std::resetiosflags(std::ios::adjustfield) << std::setiosflags(std::ios::left);
    *os << "PrintRoute at Time: " << Now().As(unit);
    *os << " from Node " << sourceNode->GetId() << " to Node " << dest->GetId();
    SimulationSingleton<GlobalRouteManagerImpl>::Get()->PrintRoute(sourceNode,
                                                                   dest,
                                                                   stream,
                                                                   nodeIdLookup,
                                                                   unit);
    (*os).copyfmt(oldState);
}

void
GlobalRouteManager::PrintRoute(Ptr<Node> sourceNode,
                               Ptr<Node> dest,
                               bool nodeIdLookup,
                               Time::Unit unit)
{
    Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper>(&std::cout);
    GlobalRouteManager::PrintRoute(sourceNode, dest, stream, nodeIdLookup, unit);
}

} // namespace ns3
