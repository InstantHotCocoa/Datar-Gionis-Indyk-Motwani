#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

struct bucket
{
	int time;//��ʱ���
	int num;//Ͱ��1����Ŀ
	struct bucket* front;
	struct bucket* next;
};

struct bucket* create(int t, int N);//��ȡ�ļ�����0��ʱ
void check(struct bucket *b, int t);//ά������ṹ
void merge(struct bucket *b);//with b->front
void show_buckets(struct bucket *b);//չʾ����Ͱ�Ļ���
int estimated_count(struct bucket *b, int t, int N);//�������ֵ

struct bucket* create(int t, int N)//��ȡ�ļ���ά������ṹ
{
	int time = 0, element;
	int accurate_count = 0;
	struct bucket *b = NULL, *old_b = NULL;
	FILE *fp = fopen("stream_sample.txt", "r");
	if (fp == NULL)
	{
		printf("��ʧ��\n");
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
			if (b->num == 1 || b->time == time)//�Է�N==0ʱ��Ұָ��b����old_b
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
	printf("��ȷֵ��%d\n", accurate_count);//show accurate value
	return b;
}

void check(struct bucket *b, int time)//ά������ṹ
{
	struct bucket *temp_b = NULL;
	while (b->front != NULL && b->front->front != NULL)//��ʣ����Ͱʱ������
	{
		if (b->num == b->front->front->num)
		{
			merge(b->front);
		}
		b = b->front;
	}
	while (b)//���������Ͱ����û�й�ʱ��Ͱ
	{
		if (b->time <= time)//discard buckets outside the sliding window
		{
			if (b->next != NULL)
			{
				b->next->front = NULL;//�Ͽ�����
			}
			while (b)//ɾ���Ͽ���֮ǰ�����нڵ�
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

void show_buckets(struct bucket *b)//չʾ����Ͱ�Ļ���
{
	int count = 1;
	for (; b->front != NULL; b = b->front)
	{
		count++;
	}
	for (; b != NULL; b = b->next)
	{
		printf("��%d��Ͱ��b->time=%d, b->num=%d\n", count, b->time, b->num);
		count--;
	}
}

int main()
{
	int t, N;
	struct bucket *tail;
	printf("������������ʱ���t��t >= 0���뻬�����ڴ�СN��N >= 0����\n");
	scanf("%d%d", &t, &N);
	tail = create(t, N);
	if (!tail)//N==0���������
	{
		printf("����ֵ��%d\n", 0);
	}
	else
	{
		show_buckets(tail);
		printf("����ֵ��%d\n", estimated_count(tail, t, N));
	}
	return 0;
}
