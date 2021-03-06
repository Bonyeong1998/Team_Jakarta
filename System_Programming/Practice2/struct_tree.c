#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>


struct StudentData {
char student_name[10];
int midterm_score;
int endterm_score;
};
struct StudentData *root = NULL;
int compare(const void *cp1, const void*cp2) {
return strcmp(((struct StudentData *)cp1)->student_name,((struct
StudentData*)cp2)->student_name);
}
void print_node(const void *nodeptr, VISIT order, int level)
{
if (order == preorder || order == leaf)
printf("Name = %-20s, middle_score = %d end_score = %d \n",(*(struct StudentData
**)nodeptr)->student_name,(*(struct StudentData **)nodeptr)->midterm_score,(*
(struct StudentData **)nodeptr)->endterm_score);
}
int main() {
int i,student_number;
struct StudentData **ret;
printf("Input student number : ");
scanf("%d",&student_number);
if(student_number <=0)
abort();
struct StudentData *tree[student_number];
for(i = 0; i<sizeof(tree) / sizeof(struct StudentData *); i++) {
tree[i] = malloc(sizeof(struct StudentData));
printf("Add username for make tree(Name, MiddleScore, EndScore) \n");
scanf("%s %d %d",tree[i]->student_name,&tree[i]->midterm_score,&tree[i]->endterm_score);
ret = (struct StudentData **) tsearch((void *) tree[i],(void **) &root,
compare);
printf("\"%s\" user ", (*ret)->student_name);
if (*ret == tree[i])
printf("Added tree\n");
else
printf("Already exist In tree.\n");
}
twalk((void *) root, print_node);
for (int i = 0; i < sizeof(tree) / sizeof(struct StudentData *); i++)
free(tree[i]);
}

