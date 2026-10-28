---
name: juce-ui
description: >
  Apply JUCE UI layout and thread ownership conventions when creating, editing,
  or reviewing source/PluginEditor.{cpp,h} or source/UI/*.{cpp,h} in Saturator.
---

# JUCE UI

## Layout

- Use `juce::FlexBox` and `juce::FlexItem` for component layout.
- Build the FlexBox tree in `resized()` and call `performLayout()` with the
  available bounds.
- Prefer flexible or proportional sizing with documented minimum and maximum
  constraints.
- Keep spacing, margins, and alignment in FlexBox properties.
- If an element requires a fixed size, document why with a short comment.
- Use nested FlexBox containers for nested sections instead of manual
  coordinate arithmetic.
- Preserve responsive behavior when the editor is resized.
- Hardcoded pixel positions with `setBounds(x, y, width, height)` are permitted
  only for temporary debugging.

## Thread Ownership

Keep UI callbacks on the message thread. Poll or asynchronously receive
audio-thread state rather than calling UI code from audio processing.

## Preferred Pattern

```cpp
void MyComponent::resized()
{
    juce::FlexBox root;
    root.flexDirection = juce::FlexBox::Direction::column;
    root.items.add(juce::FlexItem(header).withMinHeight(32.0f));
    root.items.add(juce::FlexItem(content).withFlex(1.0f));
    root.items.add(juce::FlexItem(footer).withMinHeight(24.0f));
    root.performLayout(getLocalBounds().toFloat());
}
```
