import pathlib
import matplotlib.pyplot as plt


with pathlib.Path("raw_data.txt").open() as f:
    data = list(map(float, f.readlines()))


class FilterAnna:
    def __init__(self, b, a) -> None:
        self.a = a
        self.b = b
        self.x = [0] * len(b)
        self.z = [0] * len(a)

    def process(self, val):
        res = 0

        for i in range(len(self.b) - 1):
            self.x[i + 1] = self.x[i]
        self.x[0] = val

        for i in range(len(self.b)):
            res += self.b[i] * self.x[i]

        for i in range(len(self.a)):
            res -= self.a[i] * self.z[i]

        for i in range(len(self.a) - 1):
            self.z[i + 1] = self.z[i]

        self.z[0] = res
        return res


f = FilterAnna(
    [0.007792936291952, 0.0, -0.007792936291952],
    [-1.984355370350682, 0.984414127416097],
)
data = data[90000:110000]
r = []
for d in data:
    r.append(f.process(d))

plt.plot(r)
plt.show()
