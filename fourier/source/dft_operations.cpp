#include <fourier/dft_operations.h>

using namespace wt::ft;

inline std::complex<double>
get_from_basis(const std::vector<std::complex<double>> &basis,
               const std::size_t &N)
{
  std::size_t basis_size = basis.size();
  return N < basis_size ? basis[N] : -basis[N - basis_size];
}

wt::Matrix<std::complex<double>> get_multiplier(std::size_t N)
{
  const auto i = std::complex<double>(0, 1);

  const std::complex<double> W =
      std::exp(-(2.0 * MATH_PI / static_cast<double>(N)) * 1 * i);

  // Generate the basis elements
  std::vector<std::complex<double>> basis;
  const std::size_t basis_size = N / 2;
  basis.reserve(basis_size);
  for (std::size_t i = 0; i < basis_size; ++i)
  {
    basis.push_back(std::pow(W, i));
  }

  // Generating the multiplier matrix
  wt::Matrix<std::complex<double>> multiplier{N, N};
  for (std::size_t i = 0; i < N; ++i)
  {
    auto row = multiplier[i];
    for (std::size_t c = 0; c < N; c++)
    {
      // figure out the row here
      const std::size_t basis_index = (i * c) % N;
      row[c] = get_from_basis(basis, basis_index);
    }
  }
  return multiplier;
}

std::vector<std::size_t>
partition_indices(const signal &input, const std::vector<FFT_PARTITION> &order)
{
  std::vector<std::size_t> g_index; // odd

  std::size_t g_start = 0;
  std::size_t skip = 0;

  // Calculate the offset for the starting value
  for (std::size_t i = 0; i < order.size(); ++i)
  {
    const auto &o = order[i];
    skip = static_cast<std::size_t>(1) << i;

    if (o == FFT_PARTITION::ODD)
    {
      g_start += skip;
    }
  }

  // Get the starting value
  std::size_t i = g_start;
  skip = static_cast<std::size_t>(1) << order.size();
  while (i < input.size())
  {
    g_index.push_back(i);
    i += skip;
  }

  // TEST CODE: (Add these to unit tests)
  // { n0, n1, n2, n3, n4, n5, n6, n7, .., nN } Normal                     SKIP
  // = 2^0, offset = 0 { n0, n2, n5, n6, n8, n10, n12, ..., nN } Even SKIP =
  // 2^1, offset = 0 { n2, n6, n10, n14, n18, n22, n26, ... nN} Even -> Odd SKIP
  // = 2^2, offset = 2 { n2, n10, n18, n26, n34, n42, n50, ..., nN} Even -> Odd
  // -> Even      SKIP = 2^3, offset = 2 { n10, n26, n42, n58, n76, n92, ...,
  // nN}  Even -> Odd -> Even -> Odd  SKIP = 2^4, offset = 10

  return g_index;
}

wt::Matrix<std::complex<double>>
fft_helper(const signal &input,
           const std::vector<FFT_PARTITION> &partition_order)
{
  // Need something to make sure that the length
  // is a power of 9 deaths involving the coronavirus in teaching and
  // educational pr2
  using namespace std::complex_literals;

  auto half_divisor = static_cast<std::size_t>(1) << partition_order.size();
  if ((input.size() / half_divisor) > 8)
  {
    std::vector<FFT_PARTITION> even_partition_order{partition_order};
    even_partition_order.push_back(FFT_PARTITION::EVEN);
    std::vector<FFT_PARTITION> odd_partition_order{partition_order};
    odd_partition_order.push_back(FFT_PARTITION::ODD);

    wt::Matrix<std::complex<double>> even =
        fft_helper(input, even_partition_order);
    wt::Matrix<std::complex<double>> odd =
        fft_helper(input, odd_partition_order);

    // Combine the results
    const std::size_t N = (input.size() / half_divisor);
    wt::Matrix<std::complex<double>> ret{N, 1};

    // Get the basis components for the multipliers
    // of the smaller transforms
    std::vector<std::complex<double>> basis;
    std::size_t basis_size = N >> 1;
    basis.reserve(basis_size);
    const std::complex<double> W = std::exp(
        -(2.0 * MATH_PI / static_cast<double>(N)) * std::complex<double>(0, 1));
    for (std::size_t i = 0; i < basis_size; ++i)
    {
      basis.push_back(std::pow(W, i));
    }

    // Combine the smaller transforms, as in the
    // decimation in time algorithm
    for (std::size_t i = 0; i < ret.n_rows(); ++i)
    {
      std::size_t index = i % (N >> 1);
      ret[i][0] = even[index][0] + get_from_basis(basis, i) * odd[index][0];
    }
    return ret;
  }
  else
  {
    // Partition the vector
    auto indices = partition_indices(input, partition_order);
    std::vector<std::complex<double>> partition;
    partition.reserve(input.size() / half_divisor);
    for (std::size_t i = 0; i < indices.size(); ++i)
    {
      partition.push_back(input[indices[i]]);
    }
    return slow_fft(partition);
  }
}

wt::Matrix<std::complex<double>> slow_fft(const signal &input)
{
  wt::Matrix<std::complex<double>> multiplier = get_multiplier(input.size());
  return multiplier * input;
}

wt::Matrix<std::complex<double>> fast_fft(const signal &input)
{
  return fft_helper(input, {});
}
