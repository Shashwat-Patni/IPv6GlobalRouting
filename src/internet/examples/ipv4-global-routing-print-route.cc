/*
 * Copyright (c) 2025 Shashwat Patni
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Shashwat Patni <shashwatpatni25@gmail.com>
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/test.h"

/*
*    This example demonstrates how to use Ipv4GlobalRoutingHelper::PrintRoute() and its variants to
print the route path as calculated by the global routing protocol.
*
*    ///@to-do: Finish this after all overloads are done///
*
*    Simple point to point links:
      ________
     /        \
   n0 -- n1 -- n2 -- n3     n4----n5

   n0 IP: 10.1.1.1, 10.1.4.1
   n1 IP: 10.1.1.2, 10.1.2.1
   n2 IP: 10.1.2.2, 10.1.3.1, 10.1.4.2
   n3 IP: 10.1.3.2
   n4 IP: 10.1.5.1
   n5 IP: 10:1:5:2
   Test 1:Route from n1 to n2
   expected Output:
   Route path from Node 1 to Node 2
   10.1.2.1                 (Node 1)  ---->   10.1.2.2                 (Node 2)

   Test 2: Route from n1 to n5
   expected Output:
   Time: +2s, Global Routing
   Route path from Node 1 to Node 5
   There does not exist a path from Node 1 to Node 5.

   Test 3: Route from n0 to n2
   expected Output:
   Time: +4s, Global Routing
   Route path from Node 0 to Node 2
   10.1.4.1                 (Node 0)  ---->   10.1.4.2                 (Node 2)

   Test 4: Route from n0 to n3
   expected Output:
   Route path from Node 0 to Node 3
   10.1.4.1                 (Node 0)  ---->   10.1.4.2                 (Node 2)
   10.1.4.2                 (Node 2)  ---->   10.1.3.2                 (Node 3)
*/

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Ipv4GlobalRoutingPrintRoute");

int
main(int argc, char* argv[])
{
    NodeContainer n;
    n.Create(6);
    PointToPointHelper pointToPoint;
    // globalroutinghelper to install global routing on all nodes
    Ipv4GlobalRoutingHelper globalRouting;
    InternetStackHelper stack;
    stack.SetRoutingHelper(globalRouting); // has effect on the next Install ()
    stack.Install(n);

    // NetDeviceContainers are the return type
    auto devices01 = pointToPoint.Install(n.Get(0), n.Get(1));
    auto devices12 = pointToPoint.Install(n.Get(1), n.Get(2));
    auto devices23 = pointToPoint.Install(n.Get(2), n.Get(3));
    auto devices02 = pointToPoint.Install(n.Get(0), n.Get(2));
    auto devices45 = pointToPoint.Install(n.Get(4), n.Get(5));

    // Assign IP Addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces01 = address.Assign(devices01);
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces12 = address.Assign(devices12);
    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces23 = address.Assign(devices23);
    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces02 = address.Assign(devices02);
    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces45 = address.Assign(devices45);

    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Ipv4GlobalRoutingHelper::PrintRouteAt(Seconds(0),
                                          n.Get(1),
                                          interfaces12.GetAddress(1),
                                          routingStream);

    Ipv4GlobalRoutingHelper::PrintRouteAt(Seconds(2),
                                          n.Get(1),
                                          interfaces45.GetAddress(1),
                                          routingStream);

    Ipv4GlobalRoutingHelper::PrintRouteAt(Seconds(4),
                                          n.Get(0),
                                          interfaces12.GetAddress(1),
                                          routingStream);

    Ipv4GlobalRoutingHelper::PrintRouteAt(Seconds(6),
                                          n.Get(0),
                                          interfaces23.GetAddress(0),
                                          routingStream);

    // uncomment to see all routing tables
    // Ipv4GlobalRoutingHelper::PrintRoutingTableAllAt(Seconds(3),routingStream);

    Simulator::Stop(Seconds(10));
    Simulator::Run();
    Simulator::Destroy();
}
