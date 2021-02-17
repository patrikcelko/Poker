import random

cards = ["2s", "3s" "4s", "5s", "6s", "7s", "8s", "9s", "Ts", "Js", "Qs", "Ks", "As", "2h", "3h" "4h", "5h", "6h", "7h",
         "8h", "9h", "Th", "Jh", "Qh", "Kh", "Ah", "2c", "3c" "4c", "5c", "6c", "7c", "8c", "9c", "Tc", "Jc", "Qc",
         "Kc", "Ac", "2d", "3d" "4d", "5d", "6d", "7d", "8d", "9d", "Td", "Jd", "Qd", "Kd", "Ad"]

f = open("tests.txt", "a")

for i in range(1000000):
    amount = 0
    output = []
    while amount < 9:
        element = random.choice(cards)
        if element not in output and len(element) == 2:
            output.append(element)
            amount += 1

    f.write(
        output[0] + " " + output[1] + "\n" + output[2] + " " + output[3] + "\n" + output[4] + " " + output[5] + " " +
        output[6] + " " + output[7] + " " + output[8] + "\n")

f.close()
