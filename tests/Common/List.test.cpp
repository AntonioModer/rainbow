#include "Common/List.h"

class ListTest : public testing::Test
{
protected:
	const unsigned int test_size;
	List<unsigned int> intlist;

	ListTest() : test_size(10)
	{
		// Initialise list with some content
		for (unsigned int i = 0; i < this->test_size; ++i)
			this->intlist.push_back(i);
	}
};

TEST_F(ListTest, Iterators)
{
	ASSERT_EQ(this->test_size, this->intlist.size());

	List<unsigned int>::Iterator i = this->intlist.begin();
	ASSERT_TRUE(i);
	ASSERT_EQ(this->intlist.end(), --i);
	ASSERT_FALSE(i);

	unsigned int count = 0;
	i = this->intlist.begin();
	while (count < this->test_size - 1)
	{
		ASSERT_EQ(count++, *i);
		++i;
	}
	ASSERT_EQ(count, *i);
	while (count > 0)
	{
		ASSERT_EQ(count--, *i);
		--i;
	}
	ASSERT_EQ(count, *i);
}

TEST_F(ListTest, RemoveFirsts)
{
	unsigned int i = 0;
	List<unsigned int>::Iterator iter = this->intlist.begin();
	while (iter)
	{
		ASSERT_EQ(i, *iter);
		if (++iter)
		{
			ASSERT_EQ(i + 1, *iter);
			ASSERT_EQ(i, *--iter);
		}
		this->intlist.remove(i);
		iter = this->intlist.begin();
		ASSERT_EQ(this->test_size - ++i, this->intlist.size());
	}
}

TEST_F(ListTest, RemoveMiddles)
{
	unsigned int i = 1;
	for (; i < this->test_size - 1; ++i)
	{
		this->intlist.remove(i);
		List<unsigned int>::Iterator iter = this->intlist.begin();
		ASSERT_EQ(0u, *iter);
		ASSERT_EQ(i + 1, *++iter);
		ASSERT_EQ(0u, *--iter);
	}
	ASSERT_EQ(2u, this->intlist.size());
	ASSERT_EQ(i, *++this->intlist.begin());
	ASSERT_EQ(0u, *--(++this->intlist.begin()));
	this->intlist.remove(i);
	ASSERT_EQ(1u, this->intlist.size());
	ASSERT_EQ(this->intlist.end(), ++this->intlist.begin());
	//ASSERT_DEATH_IF_SUPPORTED(--(++this->intlist.begin()), "");
	this->intlist.remove(0);
	ASSERT_EQ(0u, this->intlist.size());
	ASSERT_EQ(this->intlist.end(), this->intlist.begin());
}

TEST_F(ListTest, RemoveAll)
{
	for (unsigned int i = 0; this->intlist.size() != 0; ++i)
		this->intlist.remove(i);
	ASSERT_EQ(0u, this->intlist.size());
	ASSERT_EQ(this->intlist.end(), this->intlist.begin());
}

TEST_F(ListTest, Swap)
{
	List<unsigned int>::Iterator i1 = this->intlist.begin();
	List<unsigned int>::Iterator i2 = this->intlist.begin();
	for (unsigned int i = 0; i < this->test_size - 1; ++i)
	{
		i1.swap(++i2);
		++i1;
	}
	ASSERT_EQ(this->intlist.end(), i1);
	i1 = this->intlist.begin();
	for (unsigned int i = 1; i < this->test_size; ++i)
	{
		ASSERT_EQ(i, *i1);
		++i1;
	}
	ASSERT_EQ(0u, *i1);
	ASSERT_EQ(this->test_size, this->intlist.size());
}