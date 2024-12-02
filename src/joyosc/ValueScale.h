/*==============================================================================

	ValueScale.h

	joyosc: a device event to osc bridge

	Copyright (C) 2024 Dan Wilcox <danomatika@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.

==============================================================================*/
#pragma once

/// value scale function pointer 
typedef float (*ValueScaleFunc)(int value);

/// scale a value from one range to another
class ValueScale {

	public:

		/// normalize 16-bit HID axis values -32768 - 32767 to 0 - 1
		/// note: avoids rounding errors at slight loss in precision
		inline static float axisNorm(int value) {
			float f = (value + 32768.f) / 65535.f;
			// this is dumb, but ensures 0 -> 0.5, not 0.5000009 or similar
			return floorf(f * 100000.f) / 100000.f;
		}

		/// normalize 16-bit HID axis values -32768 - 32767 to -1 to 1
		inline static float axisSignedNorm(int value) {
			return (axisNorm(value) * 2.f) - 1.f; 
		}

		/// scale 16-bit HID axis values -32768 - 32767 to MIDI 0 - 127
		inline static float axisMidi(int value) {
			return axisNorm(value) * 127.f;
		}

		/// returns a shared axis value scaler by name or nullptr if not found
		/// name is "norm" 0 - 1, "snorm" -1 to 1, or "midi" 0 - 127
		inline static ValueScaleFunc axisScalerForName(const std::string &name) {
			if(name == "norm") {
				return &axisNorm;
			}
			else if(name == "snorm") {
				return &axisSignedNorm;
			}
			else if(name == "midi") {
				return &axisMidi;
			}
			return nullptr;
		}

		/// returns name for a shared axis value scaler or "" if not found
		inline static std::string axisScalerName(ValueScaleFunc scaler) {
			if(scaler == &axisNorm) {
				return "norm";
			}
			else if(scaler == &axisSignedNorm) {
				return "snorm";
			}
			else if(scaler == &axisMidi) {
				return "midi";
			}
			return "hid";
		}
};
