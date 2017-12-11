### This is the final for Dr. Ravindran's ECGR3090 course

All code contributions were done by:
* [Danny Jordan Murray](https://github.com/fueledbyjordan)
* [John W. Humphries](https://github.com/jhumph30)

This is quick solution to a social networking problem. The first task was to
find the top N friends of a given user. The second task was to suggest
M new mutual friends given a specific user. The algorithms used to complete
these tasks are described below.

_Note: This project should be compiled using Boost version 1.58.0.1 or greater._

##### Top N Friends
1. Find friends of the user
2. Find friends of friends
3. Build connection list
	* ID contains friend
	* mutualFriends contains number of friends shared with the user
4. Sort connection list by number of mutualFriends in descending order
5. Display top N friends from list

##### Suggest M New Friends
1. Find friends of the user
2. Find friends of friends
3. Find every user who is not a friend
4. Find intersection of notFriends and friends of friends
5. Sort intersections from step 4 in descending order by number of occurrences
6. Find top M suggestions from occurrences
