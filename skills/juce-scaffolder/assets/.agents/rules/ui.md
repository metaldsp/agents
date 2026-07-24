---
description: UI layout rules for JUCE components.
paths:
  - "source/UI/*.{cpp,h}"
globs:
  - "source/UI/*.{cpp,h}"
alwaysApply: true
applyTo: "source/UI/*.cpp,source/UI/*.h"
---

# JUCE UI Layout Rules

- Use `juce::FlexBox` and `juce::FlexItem` for responsive component layouts.
- Avoid hardcoded pixel bounds unless a control genuinely needs a fixed size; document that need.
- In `resized()`, build a FlexBox tree and call `performLayout()` with local bounds.
- Prefer flexible dimensions, margins, and alignment over manually calculated coordinates.
- Use nested FlexBox containers for nested sections and maintain responsive behavior on resize.
