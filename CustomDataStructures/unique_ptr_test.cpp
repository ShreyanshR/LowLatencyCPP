#include <gtest/gtest.h>
#include "./unique_ptr.h"

struct TestObject {
  static bool destroyed;
  ~TestObject(){ destroyed = true;}
};

bool TestObject::destroyed = false;

TEST(UniquePtrTest, ConstructorAndGet) {
  UniquePtr<int> ptr(new int(42));
  ASSERT_NE(ptr.get(), nullptr);
  EXPECT_EQ(*ptr.get(), 42);
}

TEST(UniquePtrTest, Destructor) {
  TestObject::destroyed = false;
  {
    UniquePtr<TestObject> ptr(new TestObject);
  }

  EXPECT_TRUE(TestObject::destroyed);
}

//move operator
TEST(UniquePtrTest, Move) {
  UniquePtr<int> original(new int(200));
  UniquePtr<int> moved(std::move(original));

  EXPECT_EQ(original.get(), nullptr);
  ASSERT_NE(moved.get(), nullptr);
  EXPECT_EQ(*moved, 200);
}

TEST(UniquePtrTest, MoveAssignment) {
  UniquePtr<int> original(new int(200));
  UniquePtr<int> target(new int(10));
  target = std::move(original);

  EXPECT_EQ(original.get(), nullptr);
  ASSERT_NE(target.get(), nullptr);
  EXPECT_EQ(*target, 200);
}

//REfrence to itself
TEST(UniquePtrTest, MoveSelfAssignment) {
  UniquePtr<int> ptr(new int(50));
  ptr = std::move(ptr);

  ASSERT_NE(ptr.get(), nullptr);
  EXPECT_EQ(*ptr, 50);
}

TEST(UniquePtrTest, DerefrenceOpetator) {
  UniquePtr<int> original(new int(200));
  EXPECT_EQ(*original, 200);
}

TEST(UniquePtrTest, ArrowOperator) {
  struct Test{int value;};
  UniquePtr<Test> ptr(new Test(42));

  EXPECT_EQ(ptr->value, 42);
}

TEST(UniquePtrTest, GetMethod) {
  int* raw_ptr = new int(42);
  UniquePtr<int> ptr(raw_ptr);

  EXPECT_EQ(ptr.get(), raw_ptr);
}

TEST(UniquePtrTest, ReleaseMethod) {
  UniquePtr<int> ptr(new int(56));
  auto* raw = ptr.release();

  EXPECT_EQ(ptr.get(), nullptr);
  EXPECT_EQ(*raw, 56);
  delete raw; //Manual Cleanup, why, we never assigned raw on heap, but maybe it points to an heap
}

TEST(UniquePtrTest, ResetMethod) {
  TestObject::destroyed = false;
  TestObject* first = new TestObject;

  {
    UniquePtr<TestObject> ptr(first);
    ptr.reset(new TestObject); //reset with new object
    EXPECT_TRUE(TestObject::destroyed); //first object destroyed
    TestObject::destroyed = false;
  }
  EXPECT_TRUE(TestObject::destroyed);
}

TEST(UniquePtrTest, ResetToNull) {
  UniquePtr<int> ptr(new int(56));
  ptr.reset();

  EXPECT_EQ(ptr.get(), nullptr);
}

TEST(UniquePtrTest, NullConstruct) {
  UniquePtr<int> ptr(nullptr);
  EXPECT_EQ(ptr.get(), nullptr);
}


int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}








