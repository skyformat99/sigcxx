// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>

#include <subject.hpp>
#include <observer.hpp>

using namespace std;
using CppEvent::Sender;

Test::Test()
    : testing::Test() {
}

Test::~Test() {

}

/*
 *
 */
TEST_F(Test, connect_method_once) {
  Subject s;
  Observer o;

  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE((s.CountBindings() == 0) && (s.event1().CountConnections() == 1) && (o.CountBindings() == 1));
}

/*
 *
 */
TEST_F(Test, connect_method_4_times) {
  Subject s;
  Observer o;

  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);

  s.fire_event1(1);    // this should call 4 times

  ASSERT_TRUE(o.test1_count() == 4 && s.event1().CountConnections() == 4 && o.CountBindings() == 4);
}

/*
 *
 */
TEST_F(Test, disconnect) {
  Subject s;
  Observer o;

  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().DisconnectAll(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE((o.CountBindings() == 0) && (s.event1().CountConnections() == 0));
}

/*
 *
 */
TEST_F(Test, disconnect_all) {
  Subject s;
  Observer o;

  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&o, &Observer::OnTest1IntegerParam);

  s.event1().DisconnectAll(&o, &Observer::OnTest1IntegerParam);

  s.fire_event1(1);    // nothing should be output in stdout

  ASSERT_TRUE(o.test1_count() == 0 && s.event1().CountConnections() == 0 && o.CountBindings() == 0);
}

/*
 * This testcase test the connectiong between events -- chain load
 */
TEST_F(Test, connect_event_once) {
  Subject s1;
  Subject s2;
  Observer o;

  s2.event1().Connect(&o, &Observer::OnTest1IntegerParam);
  s1.event1().Connect(s2.event1());

  s1.fire_event1(1);  // cause chain event

  ASSERT_TRUE((o.test1_count() == 1) && (s1.event1().CountConnections() == 1) && (s2.event1().CountBindings() == 1)
                  && (s2.event1().CountConnections() == 1) && (o.CountBindings() == 1));
}

/*
 * Disconnect 1
 */
TEST_F(Test, disconnect_once) {
  Subject s;
  Observer c;

  s.event1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&c, &Observer::OnTest1IntegerParam);
  s.event1().Connect(&c, &Observer::OnTest1IntegerParam);

  s.event1().DisconnectOnce(&c, &Observer::OnTest1IntegerParam, 0);
  s.event1().DisconnectOnce(&c, &Observer::OnTest1IntegerParam, -1);

  s.fire_event1(1);

  ASSERT_TRUE(c.test1_count() == 1);
}

TEST_F(Test, selfconsumer) {
  Subject s;

  s.event1().Connect(&s, &Subject::OnTest1IntegerParam);

  s.fire_event1(1);
  s.fire_event1(2);

  ASSERT_TRUE(s.test1_count() == 2);
}

TEST_F(Test, event_chaining) {
  Subject s1;
  Subject s2;
  Observer c;

  s1.event1().Connect(s2.event1());
  s2.event1().Connect(&c, &Observer::OnTest1IntegerParam);

  s1.fire_event1(1);

  ASSERT_TRUE(c.CountBindings() == 1 && c.test1_count() == 1);
}

TEST_F(Test, delete_when_called) {
  Subject s;
  Observer c1;
  Observer c2;
  Observer c3;
  Observer *obj = new Observer;

  s.event0().Connect(&c1, &Observer::OnTest0);
  s.event0().Connect(obj, &Observer::OnTestDestroy);
  s.event0().Connect(&c2, &Observer::OnTest0);
  s.event0().Connect(&c3, &Observer::OnTest0);

  s.fire_event0();

  ASSERT_TRUE((c1.test0_count() == 1) &&
      (c2.test0_count() == 1) &&
      (c3.test0_count() == 1) &&
      (s.event0().CountConnections() == 3));
}

TEST_F(Test, delete_more_when_called) {
  Subject s;
  Observer c1;
  Observer c2;
  Observer c3;
  Observer *obj1 = new Observer;
  Observer *obj2 = new Observer;
  Observer *obj3 = new Observer;

  s.event0().Connect(&c1, &Observer::OnTest0);
  s.event0().Connect(obj1, &Observer::OnTestDestroy);
  s.event0().Connect(&c2, &Observer::OnTest0);
  s.event0().Connect(&c3, &Observer::OnTest0);
  s.event0().Connect(obj2, &Observer::OnTestDestroy);
  s.event0().Connect(obj3, &Observer::OnTestDestroy);

  s.fire_event0();

  ASSERT_TRUE((c1.test0_count() == 1) &&
      (c2.test0_count() == 1) &&
      (c3.test0_count() == 1) &&
      (s.event0().CountConnections() == 3));
}

TEST_F(Test, count_event_connections) {
  Subject s1;
  Subject s2;
  Observer c;

  s1.event0().Connect(s2.event0());
  s1.event0().Connect(&c, &Observer::OnTest0);
  s1.event0().Connect(s2.event0());

  ASSERT_TRUE(
      (s1.event0().CountConnections(s2.event0()) == 2) &&
          (s1.event0().CountConnections(&c, &Observer::OnTest0) == 1)
  );
}

TEST_F(Test, unbind_all_in_callback) {
  Subject s;
  Observer o;

  s.event0().Connect(&o, &Observer::OnTestUnbindAll);
  s.event0().Connect(&o, &Observer::OnTestUnbindAll);
  s.event0().Connect(&o, &Observer::OnTestUnbindAll);
  s.event0().Connect(&o, &Observer::OnTestUnbindAll);

  s.fire_event0();
  
  ASSERT_TRUE(o.CountBindings() == 0);
}
