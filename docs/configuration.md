# Configuration
## Configuration Files

### Settings File Location

The application uses TOML format for configuration files:

- **Primary location**: `config/settings.toml`
- **User settings**: Loaded at startup, saved on exit

### TOML Format

The configuration uses TOML format:

```toml
[simulation]
max_steps_static = 15000
max_steps_moving = 30000
compute_height = 256
target_fps = 60
early_exit_distance = 5e+12
gravity_enabled = true

[graphics]
render_api = "OpenGL"
vsync_enabled = true
enable_anti_aliasing = true
show_fps = true
show_performance_metrics = true
show_debug_info = false
selected_theme = "Dark"
```

## Simulation Parameters

### Ray Tracing Settings

#### Max Steps (Static)
- **Description**: Maximum number of integration steps when camera is stationary
- **Range**: 1,000 - 30,000
- **Default**: 15,000
- **Impact**: Higher values = better accuracy, lower performance

```toml
max_steps_static = 15000
```

#### Max Steps (Moving)
- **Description**: Maximum number of integration steps when camera is moving
- **Range**: 1,000 - 60,000
- **Default**: 30,000
- **Impact**: Higher values = smoother motion, lower performance

```toml
max_steps_moving = 30000
```

#### Early Exit Distance
- **Description**: Distance at which ray marching stops to improve performance
- **Range**: 1×10¹¹ - 1×10¹³ meters
- **Default**: 5×10¹² meters
- **Impact**: Lower values = faster rendering, may miss distant objects

```toml
early_exit_distance = 5e+12
```

### Performance Settings

#### Target FPS
- **Description**: Target frame rate for the application
- **Range**: 30 - 120 FPS
- **Default**: 60 FPS
- **Impact**: Higher values = smoother animation, higher CPU usage

```toml
target_fps = 60
```

#### Compute Height
- **Description**: Resolution of the compute shader (height component)
- **Range**: 64 - 2,048 pixels
- **Default**: 256 pixels
- **Impact**: Higher values = better quality, lower performance

```toml
compute_height = 256
```

### Physics Settings

#### Gravity Enabled
- **Description**: Enable/disable gravitational interactions between objects
- **Type**: Boolean
- **Default**: true
- **Impact**: Affects object motion and orbital dynamics

```toml
gravity_enabled = true
```

## Graphics Settings

### Rendering API

#### Render API
- **Description**: Graphics API to use for rendering
- **Options**: "OpenGL", "Vulkan"
- **Default**: "OpenGL"
- **Impact**: Affects performance and feature availability

```toml
render_api = "OpenGL"
```

### Display Settings

#### V-Sync Enabled
- **Description**: Enable vertical synchronization
- **Type**: Boolean
- **Default**: true
- **Impact**: Prevents screen tearing, may limit frame rate

```toml
vsync_enabled = true
```

#### Enable Anti-Aliasing
- **Description**: Enable anti-aliasing for smoother edges
- **Type**: Boolean
- **Default**: true
- **Impact**: Better visual quality, slight performance cost

```toml
enable_anti_aliasing = true
```

### UI Settings

#### Show FPS
- **Description**: Display frame rate counter
- **Type**: Boolean
- **Default**: true
- **Impact**: Performance monitoring, minimal overhead

```toml
show_fps = true
```

#### Show Performance Metrics
- **Description**: Display detailed performance information
- **Type**: Boolean
- **Default**: true
- **Impact**: Debug information, minimal overhead

```toml
show_performance_metrics = true
```

#### Show Debug Info
- **Description**: Display debug information
- **Type**: Boolean
- **Default**: false
- **Impact**: Development information, may impact performance

```toml
show_debug_info = false
```

#### Selected Theme
- **Description**: UI theme selection
- **Options**: "Dark", "Light"
- **Default**: "Dark"
- **Impact**: Visual appearance only

```toml
selected_theme = "Dark"
```

## Performance Tuning

### Preset-Performance

The configuration system allows users to balance quality and performance:

#### High Quality Settings
```toml
[simulation]
max_steps_static = 30000
max_steps_moving = 60000
compute_height = 1024
early_exit_distance = 1e+13
```

#### Balanced Settings
```toml
[simulation]
max_steps_static = 15000
max_steps_moving = 30000
compute_height = 512
early_exit_distance = 5e+12
```

#### Performance Settings
```toml
[simulation]
max_steps_static = 5000
max_steps_moving = 10000
compute_height = 256
early_exit_distance = 2e+12
```

### Adaptive Performance

The application automatically adjusts performance based on conditions:

```glsl
// Reduce steps for distant cameras
float cameraDistance = length(cam.camPos);
if (cameraDistance > 2e12)
    maxSteps = maxSteps / 2;
else if (cameraDistance > 1e12)
    maxSteps = int(maxSteps * 0.75);

// Reduce steps for escaping rays
float initialEscapeVelocity = sqrt(2.0 * SagA_rs / ray.r);
if (ray.dr > initialEscapeVelocity * 0.95 && ray.r > SagA_rs * 200.0)
    maxSteps = maxSteps / 2;
```

## Configuration Interface

### GUI Configuration

The application provides a graphical interface for configuration:

```cpp
void ConfigState::OnImGuiRender()
{
    ImGui::Begin("Configuration");
    
    // Simulation settings
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Simulation");
    ImGui::Separator();
    
    ImGui::SliderInt("Max Steps (Static)", &m_MaxStepsStatic, 1000, 30000, "%d");
    ImGui::SliderInt("Max Steps (Moving)", &m_MaxStepsMoving, 1000, 60000, "%d");
    ImGui::SliderFloat("Early Exit Distance", &m_EarlyExitDistance, 1e11f, 1e13f, "%.2e");
    ImGui::SliderInt("Compute Height", &m_ComputeHeight, 64, 2048, "%d px");
    ImGui::SliderInt("Target FPS", &m_TargetFPS, 30, 120, "%d");
    
    // Physics settings
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Physics");
    ImGui::Separator();
    
    ImGui::Checkbox("Enable Gravity", &m_GravityEnabled);
    
    // Graphics settings
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "Graphics");
    ImGui::Separator();
    
    ImGui::Checkbox("V-Sync", &m_VSyncEnabled);
    ImGui::Checkbox("Anti-Aliasing", &m_AntiAliasingEnabled);
    ImGui::Checkbox("Show FPS", &m_ShowFPS);
    ImGui::Checkbox("Performance Metrics", &m_ShowPerformanceMetrics);
    
    ImGui::End();
}
```


## Default Configurations

### Preset Configurations

The application includes several preset configurations:

#### Ultra Quality
```toml
[simulation]
max_steps_static = 50000
max_steps_moving = 100000
compute_height = 2048
early_exit_distance = 1e+13
target_fps = 30
```

#### High Quality
```toml
[simulation]
max_steps_static = 30000
max_steps_moving = 60000
compute_height = 1024
early_exit_distance = 8e+12
target_fps = 60
```

#### Balanced
```toml
[simulation]
max_steps_static = 15000
max_steps_moving = 30000
compute_height = 512
early_exit_distance = 5e+12
target_fps = 60
```

#### Performance
```toml
[simulation]
max_steps_static = 5000
max_steps_moving = 10000
compute_height = 256
early_exit_distance = 2e+12
target_fps = 120
```

## Troubleshooting

### Common Issues

#### Performance Problems
- **Symptom**: Low frame rate, stuttering
- **Solution**: Reduce `max_steps_moving`, `max_steps_static`, or `compute_height`
- **Alternative**: Increase `early_exit_distance`

#### Quality Issues
- **Symptom**: Poor image quality, artifacts
- **Solution**: Increase `compute_height` and step counts
- **Alternative**: Reduce `early_exit_distance`

#### Configuration Errors
- **Symptom**: Application crashes on startup
- **Solution**: Delete configuration file to reset to defaults
- **Alternative**: Check TOML syntax in settings file