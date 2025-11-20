#pragma once

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <array>
#include <cassert>
#include <cmath>

namespace moco::helper {
    template<typename T>
    concept Arithmetic = requires {
        requires std::is_arithmetic_v<T>;
    };

    // TODO: See what I can figure out about SIMD
    // (C++23 experimentally includes it)
    // Could make this a whole lot easier
    
    /**
     * @brief Represents a mathematical matrix.
     * @details This class represents a mathematical
     * matrix with dimensions of M x N, and type of T,
     * defaulted to `int`.
     * This class contains overloads to perform multiple
     * matrix mathematical operations, such as:
     *  - Addition/Subtraction
     *  - Multiplication/Division by a matrix of the same dimensions
     *  - Multiplication/Division by a matrix where N and P
     *    are the same.
     * @tparam `M`: Matrix row count.
     * @tparam `N`: Matrix column count.
     * @tparam `T`: Arithmetic type that the vector holds.
     *
     */
    template<size_t M, size_t N, Arithmetic T = int>
    class Matrix {
        public:
            inline Matrix(std::initializer_list<std::initializer_list<T>> matrix) {
                assert(matrix.size() == M);
                size_t row{0};
                for (auto &r : matrix) {
                    assert(r.size() == N);
                    std::copy(r.begin(), r.end(), std::begin(m_matrix[row]));
                    row++;
                }
            }

            inline Matrix() : m_matrix() {}

            inline constexpr auto operator==(const Matrix &other) -> bool {
                return other.m_matrix == m_matrix;
            }
            inline constexpr auto operator!=(const Matrix &other) -> bool {
                return !operator==(other);
            }

            /* Has to return a matrix of the same size */

            /**
             * @brief Multiplies matricies of the same dimensions
             * using the Hadamard Product
             *
             */
            inline constexpr auto operator*=(const Matrix<M, N, T> &rhs) -> Matrix<M, N, T>& {
                for (size_t i = 0; i < M; i++) {
                    for (size_t j = 0; j < N; j++) {
                        m_matrix[i][j] *= rhs.m_matrix[i][j];
                    }
                }

                return *this;
            }
            inline friend constexpr auto operator*(Matrix<M, N, T> lhs, const Matrix<M, N, T> &rhs) -> Matrix<M, N, T> {
                lhs *= rhs;
                return lhs;
            }

            /**
             * @brief Multiplies a matrix by a constant arithmetic value
             *
             */
            inline constexpr auto operator*=(const Arithmetic auto &rhs) -> Matrix<M, N, T>& {
                for (std::array<T, N> &column : m_matrix) {
                    for (T &element : column) {
                        element *= rhs;
                    }
                }

                return *this;
            }
            inline friend constexpr auto operator*(Matrix<M, N, T> lhs, const Arithmetic auto &rhs) -> Matrix<M, N, T> {
                lhs *= rhs;
                return lhs;
            }

            /**
             * @brief Adds two matrices together.
             * @details The two matricies must have the same dimensions,
             * but don't have to have the same type, as long as there is
             * a common type for them.
             *
             */
            inline constexpr auto operator+=(const Matrix<M, N, T> &rhs) -> Matrix<M, N, T>& {
                for (size_t i = 0; i < M; i++) {
                    for (size_t j = 0; j < N; j++) {
                        m_matrix[i][j] += rhs[i][j];
                    }
                }

                return *this;
            }

            /**
             * @brief Subtracts two matrices together.
             * @details The two matricies must have the same dimensions,
             * but don't have to have the same type, as long as there is
             * a common type for them.
             *
             */
            inline constexpr auto operator-=(const Matrix<M, N, T> &rhs) -> Matrix<M, N, T>& {
                for (size_t i = 0; i < M; i++) {
                    for (size_t j = 0; j < N; j++) {
                        m_matrix[i][j] -= rhs[i][j];
                    }
                }

                return *this;
            }
            /* Array-like access */
            inline constexpr auto operator[](size_t idx) -> std::array<T, N>& {return m_matrix[idx];}
            inline constexpr auto operator[](size_t idx) const -> const std::array<T, N>& {return m_matrix[idx];}

            auto GetRowCount() const -> size_t;
            auto GetColumnCount() const -> size_t;

        private:
            std::array<std::array<T, N>, M> m_matrix;
            size_t m_rows = M;
            size_t m_columns = N;
    };
    /**
     * @brief Multiplies two vectors of different sizes
     * @details The columns of A must equal the number of rows in B.
     *
     */
    template<auto Mm, auto Nn, auto Pp, Arithmetic Tl, Arithmetic Tr>
    inline constexpr auto operator*(Matrix<Mm, Nn, Tl> lhs, const Matrix<Nn, Pp, Tr> &rhs) -> Matrix<Mm, Pp, std::common_type_t<Tl, Tr>> {
        Matrix<Mm, Pp, std::common_type_t<Tl, Tr>> newMatrix;
        for (size_t i = 0; i < Mm; i++) {
            for (size_t j = 0; j < Pp; j++) {
                for (size_t k = 0; k < Nn; k++) {
                    newMatrix[i][j] += lhs[i][k] * rhs[k][j];
                }
            }
        }

        return newMatrix;
    }

    template<auto Mm, auto Nn, Arithmetic Tl, Arithmetic Tr>
    inline constexpr auto operator-(Matrix<Mm, Nn, Tl> lhs, const Matrix<Mm, Nn, Tr> &rhs) -> Matrix<Mm, Nn, std::common_type_t<Tl, Tr>> {
        lhs -= rhs;
        return lhs;
    }

    template<auto Mm, auto Nn, Arithmetic Tl, Arithmetic Tr>
    inline constexpr auto operator+(Matrix<Mm, Nn, Tl> lhs, const Matrix<Mm, Nn, Tr> &rhs) -> Matrix<Mm, Nn, std::common_type_t<Tl, Tr>> {
        lhs += rhs;
        return lhs;
    }


    /**
     * @brief Transformation utility class
     * @detail Aids in graphics transformations
     *
     */
    class Transformation {
        public:
            inline Transformation(int x, int y) :
                m_x(x),
                m_y(y),
                m_transformationMatrix(s_defaultTransformationMatrix) {}
            inline Transformation() :
                m_x(0),
                m_y(0),
                m_transformationMatrix(s_defaultTransformationMatrix) {}
            
            inline auto GetOriginalPoints() const -> std::pair<int, int> {
                return std::make_pair(m_x, m_y);
            }

            inline auto GetTransformedPoints() const -> std::pair<int, int> {
                Matrix<3, 1> originalCoordinates{{m_x}, {m_y}, {1}};
                auto newCoordinates = m_transformationMatrix * originalCoordinates;
                return std::make_pair(newCoordinates[0][0], newCoordinates[1][0]);
            }

            inline auto Reset() -> Transformation& {
                m_transformationMatrix = s_defaultTransformationMatrix;
                return *this;
            }

            inline auto AddTranslation(int countX, int countY) -> Transformation& {
                auto translationMatrix = s_defaultTransformationMatrix;
                translationMatrix[0][2] = countX;
                translationMatrix[1][2] = countY;
                m_transformationMatrix *= translationMatrix;
                return *this;
            }

            /**
             * @brief Reflect across the X-Axis
             *
             */
            inline auto AddXReflect() -> Transformation& {
                auto reflectionMatrix = s_defaultTransformationMatrix;
                reflectionMatrix[1][1] = -1;
                m_transformationMatrix *= reflectionMatrix;
                return *this;
            }

            /**
             * @brief Reflect across the Y-Axis
             *
             */
            inline auto AddYReflect() -> Transformation& {
                auto reflectionMatrix = s_defaultTransformationMatrix;
                reflectionMatrix[0][0] = -1;
                m_transformationMatrix *= reflectionMatrix;
                return *this;
            }

            /**
             * @brief Scales the point
             *
             */
            inline auto AddScale(int xScale, int yScale) -> Transformation& {
                auto scaleMatrix = s_defaultTransformationMatrix;
                scaleMatrix[0][0] = xScale;
                scaleMatrix[1][1] = yScale;
                m_transformationMatrix *= scaleMatrix;
                return *this;
            }

            /**
             * @brief Rotates the point around the origin
             *
             */
            inline auto AddRotate(double radians) -> Transformation& {
                auto rotationMatrix = s_defaultTransformationMatrix;
                rotationMatrix[0][0] = std::cos(radians);
                rotationMatrix[0][1] = std::sin(radians) * -1;
                rotationMatrix[1][0] = std::sin(radians);
                rotationMatrix[1][1] = std::cos(radians);
                m_transformationMatrix *= rotationMatrix;
                return *this;
            }

        private:
            inline static const Matrix<3, 3> s_defaultTransformationMatrix{{1, 0, 0}, 
                                                {0, 1, 0},
                                                {0, 0, 1}};
            Matrix<3, 3> m_transformationMatrix;

            int m_x;
            int m_y;
    };
}  // namespace moco::helper
