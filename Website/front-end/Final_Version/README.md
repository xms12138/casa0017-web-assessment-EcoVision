# EcoVision Vue - Modified Version

## 修改内容 / Modifications

### 1. TEAM页面新增团队成员介绍
在 `Page4.vue` (TEAM页面) 中添加了详细的团队成员介绍部分：

**5位团队成员：**
- **Madina Diallo** - Project & Data Integration Lead 🧑‍🎓
  - Leading project coordination and managing data integration from multiple London air quality and transport sources.

- **Matilda Nelson** - Research & Web Content Lead 🧑‍💻
  - Conducting research on London's air quality challenges and developing user-focused content strategies.

- **Vidan Gao** - Design & Visualisation Lead 🎨
  - Creating intuitive visual designs and interactive data visualizations for the London borough mapping system.

- **Zihang He** - Technical & Web Development Lead 📱
  - Developing the technical infrastructure and web platform for real-time air quality data visualization.

- **Qingshan Luo** - Testing & Editing Lead ▶️
  - Ensuring quality assurance, user testing, and content editing for optimal user experience and accuracy.

**特点：**
- 每个成员卡片包含头像emoji、姓名、职位和详细描述
- 悬停效果：卡片上移并显示阴影
- 紫色和粉色渐变设计，符合整体风格
- 响应式布局，自适应不同屏幕尺寸

### 2. Submit Feedback按钮颜色改为紫色
将提交反馈按钮的颜色从蓝色 (#4285f4) 改为紫色 (#a855f7)，使其更符合整体设计风格。

**按钮样式更新：**
- 背景色：#a855f7 (紫色)
- 悬停时：#9333ea (深紫色)
- 添加了3D阴影效果，更具像素风格
- 使用 'Press Start 2P' 像素字体

### 3. 其他保持不变
- 所有其他页面(HOME、ABOUT、MAP)保持原样
- 项目结构不变
- 所有功能正常运行

## 项目结构 / Project Structure

```
ecovision-vue-modified/
├── src/
│   ├── views/
│   │   ├── Home.vue          # 首页
│   │   ├── Page2.vue         # ABOUT页面
│   │   ├── Map.vue           # MAP页面
│   │   └── Page4.vue         # TEAM页面 (已修改)
│   ├── router/
│   │   └── index.js          # 路由配置
│   ├── assets/
│   │   └── css/
│   │       └── global.css    # 全局样式
│   ├── App.vue               # 根组件
│   └── main.js               # 入口文件
├── index.html                # HTML模板
├── vite.config.js            # Vite配置
└── package.json              # 项目依赖
```

## 安装和运行 / Installation & Usage

### 1. 安装依赖
```bash
cd ecovision-vue-modified
npm install
```

### 2. 运行开发服务器
```bash
npm run dev
```

项目将在 http://localhost:3000 运行

### 3. 构建生产版本
```bash
npm run build
```

### 4. 预览生产版本
```bash
npm run preview
```

## 技术栈 / Tech Stack

- **Vue 3** - 前端框架
- **Vue Router** - 路由管理
- **Vite** - 构建工具
- **Globe.GL** - 3D地球可视化
- **CSS3** - 样式和动画

## 浏览器支持 / Browser Support

- Chrome (推荐)
- Firefox
- Safari
- Edge

## 注意事项 / Notes

1. 确保安装了 Node.js 16+ 版本
2. 首次运行需要安装依赖 (npm install)
3. 项目使用了Google Fonts和Globe.GL CDN
4. TEAM页面的Admin模式密码：ecovision2024

## 修改详情 / Modification Details

### 文件修改：
- **src/views/Page4.vue** - 主要修改文件
  - 新增 `.team-intro-section` 部分 (第36-103行)
  - 新增 `.team-members-grid` 样式 (第573-654行)
  - 修改 `.submit-btn` 按钮颜色 (第636-656行)

### 设计特点：
- 团队成员卡片采用暗色背景 + 紫色边框
- 圆形头像背景使用紫粉渐变
- 悬停时卡片上升并显示阴影效果
- 提交按钮使用像素风格的3D阴影

## License

MIT License
