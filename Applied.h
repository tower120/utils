#pragma once

namespace utils {

	class AppliedMaker;

	class Applied {
		friend AppliedMaker;
		Applied() {}

	public:
		Applied(const Applied&) = delete;
		Applied(Applied&&) = default;
	};

	class AppliedMaker {
	protected:
		static Applied makeApplied() {
			return {};
		}
	};

}