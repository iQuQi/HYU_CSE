
data = [
    {'name' : 'Bale', 'age' : 29},
    {'name' : 'Mbappe', 'age' : 20},
    {'name' : 'Gerrard', 'age' : 38},
    {'name' : 'Messi', 'age' : 31},
    {'name' : 'Ronaldo', 'age' : 34},
    {'name' : 'Neymar', 'age' : 27},
    {'name' : 'Aguero', 'age' : 30},
    {'name' : 'Kevin', 'age' : 27},
    {'name' : 'Son', 'age' : 26},
    {'name' : 'Pogba', 'age' : 26},
    {'name' : 'Park', 'age' : 38},
    {'name' : 'Silva', 'age' : 33},
    {'name' : 'Hazard', 'age' : 28},
    {'name' : 'Alonso', 'age' : 37},
    {'name' : 'Rodriguez', 'age' : 27},
    {'name' : 'Robben', 'age' : 35},
    {'name' : 'Ozil', 'age' : 30},
    {'name' : 'Modric', 'age' : 33},
    {'name' : 'Rakitic', 'age' : 31},
    {'name' : 'Suarez', 'age' : 32}
]


correct=[]

age=input("Are you in your 20s or 30s? (2 or 3) ")
name=raw_input("What is the first letter of your name? (Uppercase) ")
for i in range(0,19):
    if name in data[i]['name'] and age==(data[i]['age']/10):
        correct.append(data[i])
if len(correct)!=0:
    print(correct)
elif len(correct)==0:
    print("No such person!!!")
