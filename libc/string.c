/*
 * Phoenix-RTOS
 *
 * libc-tests
 *
 * Testing string.h functions
 *
 * Copyright 2021 Phoenix Systems
 * Author: Mateusz Niewiadomski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

/* make compilable against glibc (all these tests were run on host against libc/linux kernel) */
#define _GNU_SOURCE

#include <string.h>

#include <unity_fixture.h>


TEST_GROUP(string);


TEST_SETUP(string)
{
}


TEST_TEAR_DOWN(string)
{
}

/* Get root account details by name */
TEST(string, test_strlcpy)
{
	char source[5] = "abcd";
	char dest[5] = "klmn";
	char tmplt[5] = "xxxx";
	int i;

	/* Test full copy */
	i = strlcpy(dest, source, 5);
	TEST_ASSERT_EQUAL_INT(4, i);
	TEST_ASSERT_EQUAL_STRING(source, dest);

	/* Test reverting to template string (do not check it later) */
	i = strlcpy(dest, tmplt, 5);
	TEST_ASSERT_EQUAL_INT(4, i);
	TEST_ASSERT_EQUAL_STRING(tmplt, dest);

	/* Test shorter than source copy */
	i = strlcpy(dest, source, 3);
	TEST_ASSERT_EQUAL_STRING("ab", dest);
	TEST_ASSERT_EQUAL_INT(4, i);

	/* Test longer than source copy */
	source[3] = '\0'; /* source is now "abc" null terminated; */
	strlcpy(dest, tmplt, 5);
	i = strlcpy(dest, source, 5);
	TEST_ASSERT_EQUAL_STRING("abc", dest);
	TEST_ASSERT_EQUAL_INT(3, i);
	source[3] = 'd';

	/* Test 1 length copy */
	strlcpy(dest, tmplt, 5);
	i = strlcpy(dest, source, 1);
	TEST_ASSERT_EQUAL_STRING("", dest);
	TEST_ASSERT_EQUAL_INT(4, i);

	/* Test 1 length copy */
	strlcpy(dest, tmplt, 5);
	i = strlcpy(dest, source, 0);
	TEST_ASSERT_EQUAL_STRING(tmplt, dest);
	TEST_ASSERT_EQUAL_INT(4, i);
}

TEST(string, test_strlcat)
{
	char source1[4] = "abc";
	char source2[6] = "defgh";
	char buffer[12];
	int i;

	/* Normal, full concat to empty string */
	i = strlcat(buffer, source1, 12);
	TEST_ASSERT_EQUAL_INT(3, i);
	TEST_ASSERT_EQUAL_STRING(source1, buffer);

	/* Normal full concat to partially filled string */
	i = strlcat(buffer, source2, 12);
	TEST_ASSERT_EQUAL_INT(8, i);
	TEST_ASSERT_EQUAL_STRING("abcdefgh", buffer);

	/* Partial concat to partially filled string that should overflow the buffer */
	i = strlcat(buffer, source2, 12);
	TEST_ASSERT_EQUAL_INT(13, i);
	TEST_ASSERT_EQUAL_STRING("abcdefghdef", buffer);

	/* 1 length concat */
	buffer[6] = '\0';
	i = strlcat(buffer, source2, 1);
	TEST_ASSERT_EQUAL_INT(6, i);
	TEST_ASSERT_EQUAL_STRING("abcdef", buffer);

	/* 0 length concat */
	buffer[6] = '\0';
	i = strlcat(buffer, source2, 0);
	TEST_ASSERT_EQUAL_INT(5, i);
	TEST_ASSERT_EQUAL_STRING("abcdef", buffer);
}

TEST_GROUP_RUNNER(string)
{
	RUN_TEST_CASE(string, test_strlcpy);
	RUN_TEST_CASE(string, test_strlcat);
}
