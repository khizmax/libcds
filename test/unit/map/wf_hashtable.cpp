#include <cds_test/ext_gtest.h>
#include <cds/container/wf_hashtable.h>

#include <iostream>
#include <string>

namespace {

	namespace cc = cds::container;
	typedef cc::WfHashtable<int, double>::process* Proc;

	class WfHashtableTest : public ::testing::Test
	{
	protected:
		void SetUp()
		{
			table = new cc::WfHashtable<int, double>(10);
		}

		void TearDown()
		{
			delete table;
		}

		cc::WfHashtable<int, double>* table;
	};

	TEST_F(WfHashtableTest, process_constructor_destructor)
	{
		Proc p = table->getProcess();
		delete p;

		for (int i = 0; i < 9; ++i) {
			p = table->getProcess();
		}

		ASSERT_NO_THROW(p = table->getProcess());

		ASSERT_THROW(p = table->getProcess(), std::logic_error);
	}

	TEST_F(WfHashtableTest, insert_assign_find_delete)
	{
		Proc p = table->getProcess();

		double* v = new double[10000];
		for (int i = 0; i<10000; ++i) {
			v[i] = (double)i;
			p->insert(i, &v[i]);
		}

		ASSERT_EQ(10000, table->size());

		ASSERT_EQ(5000.0, *p->find(5000));

		double newValue = 5.0;
		p->assign(5000, &newValue);
		ASSERT_EQ(5.0, *p->find(5000));

		p->del(5000);
		ASSERT_EQ(NULL, p->find(5000));

		for (int i = 0; i < 10000; ++i) {
			p->del(i);
		}
		ASSERT_EQ(0, table->size());

		delete v;
	}

	TEST_F(WfHashtableTest, null_value)
	{
		Proc p = table->getProcess();
		p->insert(100, NULL);
		p->insert(200, NULL);
		p->insert(300, NULL);
		ASSERT_EQ(3, table->size());

		double* value = p->find(200);
		ASSERT_EQ(NULL, value);

		p->del(200);
		p->del(300);
		ASSERT_EQ(1, table->size());

	}

	TEST(TestObjectValue, read_write)
	{
		cc::WfHashtable<int, std::string>* table = new cc::WfHashtable<int, std::string>(10);
		cc::WfHashtable<int, std::string>::process* p = table->getProcess();

		std::string str1("hello libcds!");
		p->insert(2017, &str1);
		std::string* str_ref = p->find(2017);
		ASSERT_EQ(0, str_ref->compare("hello libcds!"));

		str1.append(" happy new year!");
		str_ref = p->find(2017);
		ASSERT_EQ(0, str_ref->compare("hello libcds! happy new year!"));
	}

} // namespace
