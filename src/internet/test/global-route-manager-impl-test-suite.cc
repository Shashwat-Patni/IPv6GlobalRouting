/*
 * Copyright 2007 University of Washington
 * Copyright (C) 1999, 2000 Kunihiro Ishiguro, Toshiaki Takada
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors:  Tom Henderson (tomhend@u.washington.edu)
 *
 * Kunihiro Ishigura, Toshiaki Takada (GNU Zebra) are attributed authors
 * of the quagga 0.99.7/src/ospfd/ospf_spf.c code which was ported here
 */

#include "ns3/candidate-queue.h"
#include "ns3/global-route-manager-impl.h"
#include "ns3/simulator.h"
#include "ns3/test.h"
#include "ns3/ipv4-global-routing-helper.h"
#include <cstdlib> // for rand()
#include "ns3/simple-net-device-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/node-container.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
using namespace ns3;

/**
 * @ingroup internet
 * @ingroup tests
 * @defgroup internet-test internet module tests
 */

/**
 * @ingroup internet-test
 *
 * @brief Global Route Manager Test
 */
class GlobalRouteManagerImplTestCase : public TestCase
{
  public:
    GlobalRouteManagerImplTestCase();
    void DoSetup() override;
    void DoRun() override;
    
    private:
    NodeContainer nodes;
};

GlobalRouteManagerImplTestCase::GlobalRouteManagerImplTestCase()
    : TestCase("GlobalRouteManagerImplTestCase")
{
}
void 
GlobalRouteManagerImplTestCase::DoSetup()
{
 // Manually build the link state database; four routers (0-3), 3 point-to-point
    // links
    //
    //   n0
    //      \ link 0
    //       \          link 2
    //        n2 -------------------------n3
    //       /
    //      / link 1
    //    n1
    //
    //  link0:  n0->10.1.1.1/30, n2-> 10.1.1.2/30
    //  link1: n1-> 10.1.2.1/30, n2->10.1.2.2/30
    //  link2:  n2->10.1.3.1/30, n3-> 10.1.3.2/30
    //
    
    //creating the topology
    nodes.Create(4);
    
    NodeContainer node12;
    node12.Add(nodes.Get(1));
    node12.Add(nodes.Get(2));
    
    NodeContainer node02;
    node02.Add(nodes.Get(0));
    node02.Add(nodes.Get(2));

    NodeContainer node23;
    node23.Add(nodes.Get(2));  
    node23.Add(nodes.Get(3));

    
    Ipv4GlobalRoutingHelper globalhelper;
    InternetStackHelper stack;
    stack.SetRoutingHelper(globalhelper);
    stack.Install(nodes);

    SimpleNetDeviceHelper devHelper;
    devHelper.SetNetDevicePointToPointMode(true);
    NetDeviceContainer d02;
    NetDeviceContainer d12;
    NetDeviceContainer d23;
    d02=devHelper.Install(node02);
    d12=devHelper.Install(node12);
    d23=devHelper.Install(node23);

    // Assign IP addresses to the devices
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer i02= address.Assign(d02);
    address.SetBase("10.1.2.0","255.255.255.252");
    Ipv4InterfaceContainer i12=address.Assign(d12);
    address.SetBase("10.1.3.0","255.255.255.252");
    Ipv4InterfaceContainer i23= address.Assign(d23);

}

void
GlobalRouteManagerImplTestCase::DoRun()
{
    CandidateQueue candidate;
   
    for (int i = 0; i < 100; ++i)
    {
        auto v = new SPFVertex;
        v->SetDistanceFromRoot(std::rand() % 100);
        candidate.Push(v);
    }

    for (int i = 0; i < 100; ++i)
    {
        SPFVertex* v = candidate.Pop();
        delete v;
        v = nullptr;  
    }
    NS_ASSERT_MSG(candidate.Empty()==true, "CandidateQueue should be empty after popping all elements"); 

     

    //this test is for checking the individual working of GlobalRouteManagerImpl 
    // and GlobalRouteManagerLSDB, so we will not use the GlobalRoutingHelper
    // to create the routing tables, but instead we will manually create the LSAs
    // and insert them into the GlobalRouteManagerLSDB, and then use the
    // GlobalRouteManagerImpl to calculate the routes based on the LSDB.
    // This is a manual setup of the LSAs, which would normally be done by the
    // GlobalRoutingHelper.


    // Router 0
    auto lr0 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::PointToPoint,
                                           "0.0.0.2",  // link id -> router ID 0.0.0.2
                                           "10.1.1.1", // link data -> Local IP address of router 0
                                           1);         // metric

    auto lr1 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::StubNetwork,
                                           "10.1.1.2", //link id ->adjacent neighbor's IP address
                                           "255.255.255.252",
                                           1);

    auto lsa0 = new GlobalRoutingLSA();
    lsa0->SetLSType(GlobalRoutingLSA::RouterLSA);
    lsa0->SetLinkStateId("0.0.0.0");
    lsa0->SetAdvertisingRouter("0.0.0.0");
    lsa0->SetNode(nodes.Get(0));
    lsa0->AddLinkRecord(lr0);
    lsa0->AddLinkRecord(lr1);

    // Router 1
    auto lr2 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::PointToPoint,
                                           "0.0.0.2",
                                           "10.1.2.1",
                                           1);

    auto lr3 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::StubNetwork,
                                           "10.1.2.2",
                                           "255.255.255.252",
                                           1);

    auto lsa1 = new GlobalRoutingLSA();
    lsa1->SetLSType(GlobalRoutingLSA::RouterLSA);
    lsa1->SetLinkStateId("0.0.0.1");
    lsa1->SetAdvertisingRouter("0.0.0.1");
    lsa1->AddLinkRecord(lr2);
    lsa1->AddLinkRecord(lr3);
    lsa1->SetNode(nodes.Get(1));


    // Router 2
    auto lr4 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::PointToPoint,
                                           "0.0.0.0",
                                           "10.1.1.2",
                                           1);

    auto lr5 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::StubNetwork,
                                           "10.1.1.1",
                                           "255.255.255.252",
                                           1);

    auto lr6 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::PointToPoint,
                                           "0.0.0.1",
                                           "10.1.2.2",
                                           1);

    auto lr7 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::StubNetwork,
                                           "10.1.2.2",
                                           "255.255.255.252",
                                           1);

    auto lr8 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::PointToPoint,
                                           "0.0.0.3",
                                           "10.1.3.1",
                                           1);

    auto lr9 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::StubNetwork,
                                           "10.1.3.2",
                                           "255.255.255.252",
                                           1);

    auto lsa2 = new GlobalRoutingLSA();
    lsa2->SetLSType(GlobalRoutingLSA::RouterLSA);
    lsa2->SetLinkStateId("0.0.0.2");
    lsa2->SetAdvertisingRouter("0.0.0.2");
    lsa2->AddLinkRecord(lr4);
    lsa2->AddLinkRecord(lr5);
    lsa2->AddLinkRecord(lr6);
    lsa2->AddLinkRecord(lr7);
    lsa2->AddLinkRecord(lr8);
    lsa2->AddLinkRecord(lr9);
    lsa2->SetNode(nodes.Get(2));


    // Router 3
    auto lr10 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::PointToPoint,
                                            "0.0.0.2",
                                            "10.1.3.2",
                                            1);

    auto lr11 = new GlobalRoutingLinkRecord(GlobalRoutingLinkRecord::StubNetwork,
                                            "10.1.3.1",
                                            "255.255.255.252",
                                            1);

    auto lsa3 = new GlobalRoutingLSA();
    lsa3->SetLSType(GlobalRoutingLSA::RouterLSA);
    lsa3->SetLinkStateId("0.0.0.3");
    lsa3->SetAdvertisingRouter("0.0.0.3");
    lsa3->AddLinkRecord(lr10);
    lsa3->AddLinkRecord(lr11);
    lsa3->SetNode(nodes.Get(3));

    // Test the database
    auto srmlsdb = new GlobalRouteManagerLSDB();
    srmlsdb->Insert(lsa0->GetLinkStateId(), lsa0);
    srmlsdb->Insert(lsa1->GetLinkStateId(), lsa1);
    srmlsdb->Insert(lsa2->GetLinkStateId(), lsa2);
    srmlsdb->Insert(lsa3->GetLinkStateId(), lsa3);
      
    NS_TEST_ASSERT_MSG_EQ(lsa2,
                          srmlsdb->GetLSA(lsa2->GetLinkStateId()),
                          "The Ipv4Address is not stored as the link state ID");  //LSAs are mapped by router id as key here we check that they are indeed getting the right lsa for the right key

    // next, calculate routes based on the manually created LSDB
    auto srm = new GlobalRouteManagerImpl();
    srm->DebugUseLsdb(srmlsdb); // manually add in an LSDB
    

    srm->DebugSPFCalculate(lsa0->GetLinkStateId()); // fill routing table for node n0
    
    srm->DebugSPFCalculate(lsa1->GetLinkStateId()); // fill routing table for node n1
    
    srm->DebugSPFCalculate(lsa2->GetLinkStateId()); // fill routing table for node n2

    srm->DebugSPFCalculate(lsa3->GetLinkStateId()); // fill routing table for node n3


   const std::string expectedroute02 =
   "Time: +2s, Global Routing\n"
   "Route path from Node 0 to Node 2\n"
   "10.1.1.1                 (Node 0)  ---->   10.1.1.2                 (Node 2)\n\n";
    std::ostringstream stringStream02;
    Ptr<OutputStreamWrapper> routingStream02 = Create<OutputStreamWrapper>(&stringStream02);
    Ipv4GlobalRoutingHelper::PrintRoutingPathAt(Seconds(2),nodes.Get(0),Ipv4Address("10.1.1.2"),routingStream02);
    
    std::ostringstream stringStream03;
    Ptr<OutputStreamWrapper> routingStream03 = Create<OutputStreamWrapper>(&stringStream03);
    Ipv4GlobalRoutingHelper::PrintRoutingPathAt(Seconds(3),nodes.Get(0),Ipv4Address("10.1.3.2"),routingStream03);
    //Ipv4GlobalRoutingHelper::PrintRoutingTableAllAt(Seconds(2),  routingStream03);
const std::string expectedroute03 =
   "Time: +3s, Global Routing\n"
   "Route path from Node 0 to Node 3\n"
   "10.1.1.1                 (Node 0)  ---->   10.1.1.2                 (Node 2)\n"
   "10.1.3.1                 (Node 2)  ---->   10.1.3.2                 (Node 3)\n\n";
    
    Simulator::Run();


    //-----------------Now the tests------------------
    
    //test 1 check if the SPF calculate Sets default routes for Stub nodes 
    NS_TEST_EXPECT_MSG_EQ(stringStream02.str(), expectedroute02, "The default route from node 0 to node 2 is not correct.GlobalroutemanagerImpl doesn't install the correct default route for stub node");
    
    //test 2 check route from node 0 to node 3 
    NS_TEST_EXPECT_MSG_EQ(stringStream03.str(), expectedroute03, "The route from node 0 to node 3 is not correct. GlobalRouteManagerImpl doesn't install the correct route for node 3");
     

    Simulator::Stop(Seconds(3));
    Simulator::Destroy();

    // This delete clears the srm, which deletes the LSDB, which clears
    // all of the LSAs, which each destroys the attached LinkRecords.
    delete srm;

}













/**
 * @ingroup internet-test
 *
 * @brief Global Route Manager TestSuite
 */
class GlobalRouteManagerImplTestSuite : public TestSuite
{
  public:
    GlobalRouteManagerImplTestSuite();

  private:
};

GlobalRouteManagerImplTestSuite::GlobalRouteManagerImplTestSuite()
    : TestSuite("global-route-manager-impl", Type::UNIT)
{
    AddTestCase(new GlobalRouteManagerImplTestCase(), TestCase::Duration::QUICK);
}

static GlobalRouteManagerImplTestSuite
    g_globalRoutingManagerImplTestSuite; //!< Static variable for test initialization
