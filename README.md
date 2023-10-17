# Adaptive Partitioning Mututal Information (aminfo)

## Introduction

This package is an implementation of the adaptive partitioning mutual information algorithm [1]. This project is packaged for use in Python using Pybind11 and is written in standard C++. 

The adaptive partitioning algorithm is a single parameter estimator of the mutual information between two continuous variables. This package is untested on discrete (categorical data).

## Installation

The package is installed via `pip` in the traditional way.

```bash
pip install aminfo
```

## Usage

First, you should have two continuous variables `X` and `Y` that you seek to find the mutual information from. These variables should be `NaN` free.
```python
from aminfo import adaptive_mutual_information

X, Y = generate_some_data(...)

MI = adaptive_mutual_information(X, Y)
```

The function accepts a third positional argument that represents the only parameter used in this estimator, the critical Chi-squared value. This value is set to 8.0 by default.

```python
MI = adaptive_mutual_information(X, Y, 8.0)
```

## Background
### Adaptive Partitioning
The adaptive partitioning algorithm is an improvement on the "histogram" method of calculating mutual information. In this method, the bin sizes are driven by the information latent in distribution. The algorithm works by recursively partitioning the distribution in a fashion very similar to quadtree traversal. Each "child" is tested for distribution uniformity through a Chi-squared test:

\[
\chi^2 = \sum_{i=0}^{3} \frac{ \left( \left| n_i - e_i \right| - 0.5 \right)^2}{e_i}
\]

The child cell can split into grandchildren if the statistic exceeds some critical value:

\[
\chi^2 > \chi^2_c
\]

Typical values to try for `chi` are in the range of 4.0-8.0 for most distributions. A higher `chi` will make it harder to split each child and find more relationship between the variables, thereby having a tendency to underestimate the true MI. A lower `chi` will make it easier to split each child cell and may find more information than is really there, causing a tendency to overestimate the true MI. 

## Images

You can include images from your `img` folder like this:

![Sample Image](img/sample_image.png)

## Usage

Here's how you can use this package:

```python
import your_package

# Your code here
```

## License

MIT License

## Acknowledgments

- Acknowledgment 1
- Acknowledgment 2
