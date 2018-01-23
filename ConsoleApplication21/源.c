#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

struct bucket
{
	int time;//右时间点
	int num;//桶中1的数目
	struct bucket* front;
	struct bucket* next;
};

struct bucket* create(int t, int N);//读取文件，从0计时
void check(struct bucket *b, int t);//维护链表结构
void merge(struct bucket *b);//with b->front
void show_buckets(struct bucket *b);//展示所有桶的划分
int estimated_count(struct bucket *b, int t, int N);//计算估计值

struct bucket* create(int t, int N)//读取文件，维护链表结构
{
	int time = 0, element;
	int accurate_count = 0;
	struct bucket *b = NULL, *old_b = NULL;
	FILE *fp = fopen("stream_sample.txt", "r");
	if (fp == NULL)
	{
		printf("打开失败\n");
		return NULL;
	}

	for (; !feof(fp) && time <= t; time++)
	{
		fscanf(fp, "%d", &element);
		if (element == 1)
		{
			if (time > t - N)
			{
				accurate_count++;//accurate value counter
			}
			b = (struct bucket*)malloc(sizeof(struct bucket));
			b->num = 1;
			b->time = time;
			b->front = NULL;
			b->next = NULL;
			if (old_b != NULL)
			{
				old_b->next = b;
				b->front = old_b;
			}
			check(b, time - N);
			if (b->num == 1 || b->time == time)//以防N==0时，野指针b赋给old_b
			{
				old_b = b;
			}
			else
			{
				b = NULL;
			}
		}
	}
	fclose(fp);
	printf("精确值：%d\n", accurate_count);//show accurate value
	return b;
}

void check(struct bucket *b, int time)//维护链表结构
{
	struct bucket *temp_b = NULL;
	while (b->front != NULL && b->front->front != NULL)//仅剩两个桶时无需检查
	{
		if (b->num == b->front->front->num)
		{
			merge(b->front);
		}
		b = b->front;
	}
	while (b)//检查这两个桶中有没有过时的桶
	{
		if (b->time <= time)//discard buckets outside the sliding window
		{
			if (b->next != NULL)
			{
				b->next->front = NULL;//断开链表
			}
			while (b)//删除断开点之前的所有节点
			{
				temp_b = b->front;
				free(b);
				b = temp_b;
			}
		}
		else
		{
			b = b->front;
		}
	}
}

void merge(struct bucket *b)//with b->front
{
	struct bucket *temp = b->front;
	b->num *= 2;
	b->front = temp->front;
	if (temp->front)
	{
		temp->front->next = temp->next;
	}
	free(temp);
}

int estimated_count(struct bucket *b, int t, int N)//calculate estimated value
{
	int count = 0;
	for (; b != NULL; b = b->front)
	{
		if (b->front == NULL)
		{
			count += b->num / 2;
		}
		else
		{
			count += b->num;
		}
	}
	return count;
}

void show_buckets(struct bucket *b)//展示所有桶的划分
{
	int count = 1;
	for (; b->front != NULL; b = b->front)
	{
		count++;
	}
	for (; b != NULL; b = b->next)
	{
		printf("第%d个桶：b->time=%d, b->num=%d\n", count, b->time, b->num);
		count--;
	}
}

int main()
{
	int t, N;
	struct bucket *tail;
	printf("依次输入任意时间点t（t >= 0）与滑动窗口大小N（N >= 0）：\n");
	scanf("%d%d", &t, &N);
	tail = create(t, N);
	if (!tail)//N==0的特殊情况
	{
		printf("估计值：%d\n", 0);
	}
	else
	{
		show_buckets(tail);
		printf("估计值：%d\n", estimated_count(tail, t, N));
	}
	return 0;
}
