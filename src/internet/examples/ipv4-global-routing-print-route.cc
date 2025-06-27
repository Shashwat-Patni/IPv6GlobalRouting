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

/**
*    This example demonstrates how to use Ipv4GlobalRoutingHelper::PrintRoute() , PrintRoutePathAt()
and its overloads to
  print the route path as calculated by the global routing protocol. The Print Format loosely
follows that of traceRoute.
   Similar to TraceRoute, There is an option to disable the Reverse Node ID lookup.
*We have two Functions PrintRoute() and PrintRouteAt(). Each of these have 4 overloads.


* PrintRoute(): Prints the route from:
                * Source node  to destination node pointer given a output stream
                *  Source Node to a destination Ip given a output stream
                The other two overloads are for defaulting to the std::Cout output stream

  PrintRouteAt(): Similar to PrintRoute but takes in a printTime argument to determine when in the
                  simulation the output is printed. Similar overloads:
                  * Source node  to destination node pointer given a output stream
                  * Source Node to a destination Ip given a output stream
                  The other two overloads are for defaulting to the std::Cout output stream

  The default behaviour is to also print the reverse node id. This can be disabled by setting the
  NodeIdLookup argument to false.this is similar to linux traceroute -n option.

  We explain the use of each of the overloads for PrintRoute() using the below topology
  The At variant behaves similarly with the added time argument.



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

  Test 1:Route from n1 to 10.1.2.2
  expected Output:
  PrintRoute at Time: +0s from Node 1 to address 10.1.2.2, 64 hops Max.
   1  10.1.2.2 (Node 2)

  Test 2: Route from n1 to n3
  expected Output:
  PrintRoute at Time: +0s from Node 1 to Node 3, 64 hops Max.
   1  10.1.2.2 (Node 2)
   2  10.1.3.2 (Node 3)

  Test 3: Route from n0 to 10.1.5.2
  expected Output:
  PrintRoute at Time: +0s from Node 0 to address 10.1.5.2, 64 hops Max.
  There does not exist a path from Node 0 to Node 5.

  Test 4: Route from n0 to n3
  expected Output:
  PrintRoute at Time: +0s from Node 0 to Node 3, 64 hops Max.
   1  10.1.4.2 (Node 2)
   2  10.1.3.2 (Node 3)

  Test 5: Route from n0 to 10.1.2.2
  expected Output:
  PrintRoute at Time: +0s from Node 0 to address 10.1.2.2, 64 hops Max.
   1  10.1.4.2
      10.1.2.2

  Test 6: Route from n0 to n3
  expected Output:
  PrintRoute at Time: +0s from Node 0 to Node 3, 64 hops Max.
   1  10.1.4.2
   2  10.1.3.2

  Test 7: Route from n1 to 10.1.2.2 at time +2s
  expected Output:
  PrintRoute at Time: +2s from Node 1 to address 10.1.2.2, 64 hops Max.
   1  10.1.2.2 (Node 2)

  Test 8: Route from n1 to n3 at time +4s
  expected Output:
  PrintRoute at Time: +4s from Node 1 to Node 3, 64 hops Max.
   1  10.1.2.2 (Node 2)
   2  10.1.3.2 (Node 3)
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

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // create an output stream to be used in the examples
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);

    //----------------------Now the examples-----------------------

    // PrintRoute(): Route from:  Source node  to destination IP Address given an output stream
    Ipv4GlobalRoutingHelper::PrintRoute(n.Get(1), interfaces12.GetAddress(1), routingStream);

    // PrintRoute(): Route From : Source node to Destination Node given an output stream
    Ipv4GlobalRoutingHelper::PrintRoute(n.Get(1), n.Get(3), routingStream);

    // Variant with output stream as std::cout by default
    Ipv4GlobalRoutingHelper::PrintRoute(n.Get(0), interfaces45.GetAddress(1));

    Ipv4GlobalRoutingHelper::PrintRoute(n.Get(0), n.Get(3));

    //  PrintRoute() with nodeIdLookup disabled
    Ipv4GlobalRoutingHelper::PrintRoute(n.Get(0), interfaces12.GetAddress(1), false);
    // also provide the output stream
    Ipv4GlobalRoutingHelper::PrintRoute(n.Get(0), n.Get(3), routingStream, false);

    // The At Variants of PrintRoute() behave similarly with the added time argument.
    Ipv4GlobalRoutingHelper::PrintRouteAt(Seconds(2), n.Get(1), interfaces12.GetAddress(1));
    Ipv4GlobalRoutingHelper::PrintRouteAt(Seconds(4), n.Get(1), n.Get(3));

    // uncomment to see all routing tables
    // Ipv4GlobalRoutingHelper::PrintRoutingTableAllAt(Seconds(3),routingStream);

    Simulator::Stop(Seconds(10));
    Simulator::Run();
    Simulator::Destroy();
}
