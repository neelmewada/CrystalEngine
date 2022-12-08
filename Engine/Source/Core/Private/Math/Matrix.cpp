
#include "Math/Matrix.h"

namespace CE
{

	Matrix4x4 Matrix4x4::operator+(const Matrix4x4& rhs) const
	{
		Matrix4x4 result{};
		
		for (int i = 0; i < 4; i++)
		{
			result.Rows[i] = Rows[i] + rhs.Rows[i];
		}

		return result;
	}

	Matrix4x4 Matrix4x4::operator-(const Matrix4x4& rhs) const
	{
		Matrix4x4 result{};

		for (int i = 0; i < 4; i++)
		{
			result.Rows[i] = Rows[i] - rhs.Rows[i];
		}

		return result;
	}

	Matrix4x4 Matrix4x4::operator*(f32 rhs) const
	{
		Matrix4x4 result{};

		for (int i = 0; i < 4; i++)
		{
			result.Rows[i] *= rhs;
		}

		return result;
	}

	Matrix4x4 Matrix4x4::operator/(f32 rhs) const
	{
		Matrix4x4 result{};

		for (int i = 0; i < 4; i++)
		{
			result.Rows[i] /= rhs;
		}

		return result;
	}

	Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& lhs, const Matrix4x4& rhs)
	{
		Matrix4x4 result{};

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

	Matrix4x4 Matrix4x4::GetTranspose(const Matrix4x4& mat)
	{
		Matrix4x4 result{};

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.Rows[i][j] = mat.Rows[j][i];
			}
		}

		return result;
	}

	void Matrix4x4::GetCofactor(const Matrix4x4& mat, Matrix4x4& cofactor, s32 p, s32 q, s32 n)
	{
		s32 i = 0, j = 0;

		for (int row = 0; row < n; row++) 
		{
			for (int col = 0; col < n; col++) 
			{
				if (row != p && col != q) 
				{
					cofactor[i][j++] = mat[row][col];

					// Row is filled, so increase row index and
					// reset col index
					if (j == n - 1) 
					{
						j = 0;
						i++;
					}
				}
			}
		}
	}

	int Matrix4x4::GetDeterminant(const Matrix4x4& mat, s32 n)
	{
		int determinant = 0;

		if (n == 1)
			return mat[0][0];

		Matrix4x4 temp{};

		int sign = 1;

		// Iterate for each element of first row
		for (s32 f = 0; f < n; f++) 
		{
			// Getting Cofactor of A[0][f]
			GetCofactor(mat, temp, 0, f, n);
			determinant += sign * mat[0][f] * GetDeterminant(temp, n - 1);

			// terms are to be added with alternate sign
			sign = -sign;
		}

		return determinant;
	}

	Matrix4x4 Matrix4x4::GetAdjoint(const Matrix4x4& mat)
	{
		Matrix4x4 adj{};

		// temp is used to store cofactors of mat[][]
		Matrix4x4 temp{};

		int sign = 1;

		for (int i = 0; i < 4; i++) 
		{
			for (int j = 0; j < 4; j++) 
			{
				// Get cofactor of A[i][j]
				GetCofactor(mat, temp, i, j, 4);

				// sign of adj[j][i] positive if sum of row
				// and column indexes is even.
				sign = ((i + j) % 2 == 0) ? 1 : -1;

				// Interchanging rows and columns to get the
				// transpose of the cofactor matrix
				adj[j][i] = (sign) * (GetDeterminant(temp, 4 - 1));
			}
		}

		return adj;
	}

	Matrix4x4 Matrix4x4::GetInverse(const Matrix4x4& mat)
	{
		Matrix4x4 inverse{};

		// Find determinant of A[][]
		int det = GetDeterminant(mat, 4);
		if (det == 0) 
		{
			CE_LOG(Error, All, "Singular matrix given to GetInverse function (Invalid)");
			return Matrix4x4::Zero();
		}

		// Find adjoint
		auto adj = GetAdjoint(mat);

		// Find Inverse using formula "inverse(A) =
		// adj(A)/det(A)"
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				inverse[i][j] = adj[i][j] / (f32)det;

		return inverse;
	}


} // namespace CE

