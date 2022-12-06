
#include "Math/Matrix.h"

namespace CE
{

	Matrix Matrix::operator+(const Matrix& rhs)
	{
		Matrix result{};
		
		for (int i = 0; i < 4; i++)
		{
			result.Rows[i] = Rows[i] + rhs.Rows[i];
		}

		return result;
	}

	Matrix Matrix::operator-(const Matrix& rhs)
	{
		Matrix result{};

		for (int i = 0; i < 4; i++)
		{
			result.Rows[i] = Rows[i] - rhs.Rows[i];
		}

		return result;
	}

	Matrix Matrix::operator*(f32 rhs)
	{
		Matrix result{};

		for (int i = 0; i < 4; i++)
		{
			result.Rows[i] *= rhs;
		}

		return result;
	}

	Matrix Matrix::operator/(f32 rhs)
	{
		Matrix result{};

		for (int i = 0; i < 4; i++)
		{
			result.Rows[i] /= rhs;
		}

		return result;
	}

	Matrix Matrix::Multiply(const Matrix& lhs, const Matrix& rhs)
	{
		Matrix result{};

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.Rows[i][j] = 0;

				for (int k = 0; k < 4; k++) 
				{
					result.Rows[i][j] += lhs.Rows[i][k] * rhs.Rows[k][j];
				}
			}
		}

		return result;
	}

	Matrix Matrix::Transpose(const Matrix& mat)
	{
		Matrix result{};

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.Rows[i][j] = mat.Rows[j][i];
			}
		}

		return result;
	}

} // namespace CE

