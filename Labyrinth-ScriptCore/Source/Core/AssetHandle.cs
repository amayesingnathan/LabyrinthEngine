using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
	public struct AssetHandle
	{
		public static readonly AssetHandle Invalid = new AssetHandle(0);

		private ulong mHandle;

		public AssetHandle(ulong handle) { mHandle = handle; }

		public bool IsValid() => Internal.AssetHandle_IsValid(ref this);

		public static implicit operator bool(AssetHandle assetHandle)
		{
			return Internal.AssetHandle_IsValid(ref assetHandle);
		}

		public override string ToString() => mHandle.ToString();
	}
}
