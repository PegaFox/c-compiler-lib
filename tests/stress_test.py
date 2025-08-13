import random
import subprocess

TEST_COUNT = 1000


def createRandomFile(filename: str):
    with open(filename, 'x') as randomFile:
        fileData = bytearray()
        for c in range(random.randint(0, 10000)):
            fileData.append(random.randint(0, 127))

        randomFile.write(fileData.decode("ascii"))


print("Generating tests")
for f in range(TEST_COUNT):
    createRandomFile("stress_tests/test_"+str(f)+".c")

for f in range(TEST_COUNT):
    print("Running test "+str(f))
    subprocess.run(["../build/main", "stress_tests/test_"+str(f)+".c"])
