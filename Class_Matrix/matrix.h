#include "biginteger.h"

template <int N, int div, bool is_less_sqrt>
struct SqrtHelper {
    static const int value =
        SqrtHelper<N, div + 1, (N / (div + 1) > div + 1)>::value;
};

template <int N, int Div>
struct SqrtHelper<N, Div, false> {
    static const int value = Div;
};

template <int N>
struct Sqrt {
    static const int value = SqrtHelper<N, 1, true>::value;
};

template <int N, int divisor>
struct PrimeHelper {
    static const bool value =
        ((N % divisor != 0) && PrimeHelper<N, divisor - 1>::value);
};

template <int N>
struct PrimeHelper<N, 1> {
    static const bool value = true;
};

template <int N>
struct IsPrime {
    static const bool value = (PrimeHelper<N, Sqrt<N>::value>::value);
};

template <>
struct IsPrime<1> {
    static const bool value = false;
};

// class Residue
template <int N>
class Residue {
  public:
    int val = 0;
    Residue() = default;
    explicit Residue(int a) {
        val = (a % static_cast<int>(N) + N) % N;
    }
    explicit operator int() const {
        return val;
    }

    Residue<N>& operator+=(const Residue<N>& a) {
        val += a.val;
        val %= N;
        return *this;
    }

    Residue<N> operator+(const Residue<N>& a) const {
        return Residue<N>(a.val + val);
    }

    Residue<N>& operator-=(const Residue<N>& a) {
        val = (val - a.val + N) % N;
        return *this;
    }

    Residue<N> operator-(const Residue<N>& a) const {
        Residue<N> answ(val - a.val);
        return answ;
    }

    Residue<N>& operator*=(const Residue<N>& a) {
        val *= a.val;
        val %= N;
        return *this;
    }

    Residue<N> operator*(const Residue<N>& a) const {
        return Residue<N>(a.val * val);
    }

    Residue<N>& operator/=(const Residue<N>& a) {
        static_assert(IsPrime<N>::value == true);
        val *= fst_pow(a.val, N - 2);
        val %= N;
        return *this;
    }

    Residue<N> operator/(const Residue<N>& a) const {
        static_assert(IsPrime<N>::value == true);
        Residue<N> ans(val);
        ans /= a;
        return ans;
    }

    Residue<N> operator-() const {
        return Residue<N>((static_cast<int>(N) - val) % N);
    }

    bool operator==(const Residue<N>& a) const {
        return val == a.val;
    }

    bool operator!=(const Residue<N>& a) const {
        return !(val == a.val);
    }
    string toString() const {
        return std::to_string(val);
    }

  private:
    int fst_pow(int x, int y) {
        if (y == 0) {
            return 1;
        }
        long long t = fst_pow(x, y / 2);
        if (y % 2 == 1) {
            return (((t * t) % N) * x) % N;
        }
        return (t * t) % N;
    }
};

template <int N>
std::ostream& operator<<(std::ostream& out, const Residue<N>& dig) {
    out << dig.val;
    return out;
}

// class Matrix

template <int M, int N, typename Field = Rational>
class Matrix {
  public:
    Matrix();
    Matrix(std::initializer_list<std::initializer_list<int>> sta);
    Matrix<M, N, Field>& operator*=(Field a);
    Matrix<M, N, Field> operator*(Field a) const;

    Matrix<M, N, Field>& operator*=(Matrix<M, N, Field> other);

    Matrix<M, N, Field>& operator+=(const Matrix<M, N, Field>& newMa);
    Matrix<M, N, Field> operator+(const Matrix<M, N, Field>& newMa) const;
    Matrix<M, N, Field>& operator-=(const Matrix<M, N, Field>& newMa);
    Matrix<M, N, Field> operator-(const Matrix<M, N, Field>& newMa) const;
    std::array<Field, N>& operator[](int i);
    const std::array<Field, N>& operator[](int i) const;

    bool operator==(const Matrix<M, N, Field>& another) const;
    bool operator!=(const Matrix<M, N, Field>& another) const;

    Matrix<N, M, Field> transposed() const;
    std::array<Field, N> getRow(unsigned int id) const;

    std::array<Field, M> getColumn(unsigned int id) const;

    Field trace() const;
    int rank() const;

    Field det() const;

    Matrix<M, N, Field>& invert();

    Matrix<M, N, Field> inverted() const;

  private:
    std::array<std::array<Field, N>, M> ma;
    void add_row_to_row(int id_from, int id_to, Field mult = Field(1));
    Matrix gauss();
};

template <int N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;

template <int N>
constexpr int Sqrt_v = Sqrt<N>::value;

template <int M, int N, typename Field>
Matrix<M, N, Field>::Matrix() {}

template <int M, int N, typename Field>
Matrix<M, N, Field>::Matrix(
    std::initializer_list<std::initializer_list<int>> sta) {
    int i = 0, j = 0;
    for (auto to : sta) {
        j = 0;
        for (auto x : to) {
            ma[i][j] = Field(x);
            j++;
        }
        i++;
    }
}

template <int M, int N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(const Field a) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            ma[i][j] *= a;
        }
    }
    return *this;
}

template <int M, int N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::operator*(const Field a) const {
    Matrix<M, N, Field> answ = *this;
    answ *= a;
    return answ;
}

template <int M, int N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(
    const Matrix<M, N, Field> other) {
    static_assert(N == M, "Dimensions don't match!");
    *this = (*this * other);
    return *this;
}

template <int M, int N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator+=(
    const Matrix<M, N, Field>& newMa) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            ma[i][j] += newMa[i][j];
        }
    }
    return *this;
}

template <int M, int N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::operator+(
    const Matrix<M, N, Field>& newMa) const {
    Matrix<M, N, Field> answ = *this;
    answ += newMa;
    return answ;
}

template <int M, int N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator-=(
    const Matrix<M, N, Field>& newMa) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            ma[i][j] -= newMa[i][j];
        }
    }
    return *this;
}

template <int M, int N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::operator-(
    const Matrix<M, N, Field>& newMa) const {
    Matrix<M, N, Field> answ = *this;
    answ -= newMa;
    return answ;
}
template <int M, int N, typename Field>
std::array<Field, N>& Matrix<M, N, Field>::operator[](int i) {
    return ma[i];
}
template <int M, int N, typename Field>
const std::array<Field, N>& Matrix<M, N, Field>::operator[](int i) const {
    return ma[i];
}

template <int M, int N, typename Field>
bool Matrix<M, N, Field>::operator==(const Matrix<M, N, Field>& another) const {
    for (int i = 0; i < M; i++) {
        if (ma[i] != another[i]) {
            return false;
        }
    }
    return true;
}

template <int M, int N, typename Field>
bool Matrix<M, N, Field>::operator!=(const Matrix<M, N, Field>& another) const {
    return !(*this == another);
}

template <int M, int N, typename Field>
Matrix<N, M, Field> Matrix<M, N, Field>::transposed() const {
    Matrix<N, M, Field> ans;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            ans[j][i] = ma[i][j];
        }
    }
    return ans;
}
template <int M, int N, typename Field>
std::array<Field, N> Matrix<M, N, Field>::getRow(unsigned int id) const {
    return ma[id];
}
template <int M, int N, typename Field>
std::array<Field, M> Matrix<M, N, Field>::getColumn(unsigned int id) const {
    std::array<Field, M> ans;
    for (int i = 0; i < M; i++) {
        ans[i] = ma[i][id];
    }
    return ans;
}
template <int M, int N, typename Field>
Field Matrix<M, N, Field>::trace() const {
    static_assert(N == M);
    Field ans;
    for (int i = 0; i < M; i++) {
        ans += ma[i][i];
    }
    return ans;
}
template <int M, int N, typename Field>
void Matrix<M, N, Field>::add_row_to_row(int id_from, int id_to, Field mult) {
    for (int i = 0; i < N; i++) {
        ma[id_to][i] += (ma[id_from][i] * mult);
    }
}

template <int M, int N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::gauss() {
    int last_fixed_row = -1, fl = 0;
    for (int i = 0; i < static_cast<int>(N); ++i) {
        for (int j = last_fixed_row + 1; j < static_cast<int>(M); ++j) {
            if (ma[j][i] == Field(0)) {
                continue;
            }
            if (j != last_fixed_row + 1 && fl == 0) {
                add_row_to_row(j, last_fixed_row + 1);
            }
            ++last_fixed_row;
            for (int k = 0; k < static_cast<int>(M); ++k) {
                if (ma[k][i] != Field(0) && last_fixed_row != k) {
                    add_row_to_row(last_fixed_row, k,
                                   (-ma[k][i]) / ma[last_fixed_row][i]);
                }
            }
            break;
        }
    }
    return (*this);
}

template <int M, int N, typename Field>
int Matrix<M, N, Field>::rank() const {
    Matrix<M, N, Field> help = *this;
    help.gauss();
    int cnt = 0;
    for (int i = 0; i < M; i++) {
        int fl1 = 0;
        for (int j = 0; j < N; j++) {
            if (help[i][j] != Field(0)) {
                fl1 = 1;
            }
        }
        cnt += fl1;
    }
    return cnt;
}

template <int M, int N, typename Field>
Field Matrix<M, N, Field>::det() const {
    static_assert(N == M);
    Matrix<M, N, Field> help = *this;
    help = help.gauss();
    Field ans(1);
    for (int i = 0; i < M; i++) {
        ans *= help[i][i];
    }
    return ans;
}

template <int M, int N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::invert() {
    static_assert(M == N);
    *this = inverted();
    return *this;
}
template <int M, int N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::inverted() const {
    static_assert(M == N);
    Matrix<M, N + N, Field> help;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < 2 * N; j++) {
            if (j < N) {
                help[i][j] = ma[i][j];
            } else {
                if (j - N == i) {
                    help[i][j] = Field(1);
                } else {
                    help[i][j] = Field(0);
                }
            }
        }
    }
    help.gauss();
    Matrix<M, N, Field> ans;
    for (int i = 0; i < M; i++) {
        Field koef = help[i][i];
        for (int j = N; j < N + N; j++) {
            ans[i][j - N] = help[i][j] / koef;
        }
    }
    return ans;
}

template <int N, int M, typename Field>
Matrix<M, N, Field> operator*(const Field a, const Matrix<M, N, Field> mat) {
    Matrix<M, N, Field> answ = mat;
    answ *= a;
    return answ;
}

template <int M, int N, int K, typename Field>
Matrix<M, N, Field> operator*(const Matrix<N, K, Field>& ma,
                              const Matrix<N, K, Field>& newMa) {
    Matrix<M, K, Field> ans;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            Field hlp(0);
            for (int e = 0; e < N; e++) {
                hlp += ma[i][e] * newMa[e][j];
            }
            ans[i][j] = hlp;
        }
    }
    return ans;
}