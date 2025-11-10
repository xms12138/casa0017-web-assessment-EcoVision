# 快速开始指南 / Quick Start Guide

## 修改摘要 / Summary of Changes

✅ **已完成的修改：**

1. **TEAM页面 - 新增5位团队成员介绍**
   - Madina Diallo (Project & Data Integration Lead) 🧑‍🎓
   - Matilda Nelson (Research & Web Content Lead) 🧑‍💻
   - Vidan Gao (Design & Visualisation Lead) 🎨
   - Zihang He (Technical & Web Development Lead) 📱
   - Qingshan Luo (Testing & Editing Lead) ▶️

2. **Submit Feedback按钮颜色改为紫色 (#a855f7)**
   - 符合整体紫色主题设计
   - 添加了像素风格的3D阴影效果

3. **其他页面保持不变**
   - HOME页面：全球空气质量监测
   - ABOUT页面：项目介绍
   - MAP页面：交互式地图占位

## 30秒快速启动 / Quick Start in 30 Seconds

```bash
# 1. 解压项目
tar -xzf ecovision-vue-modified.tar.gz
cd ecovision-vue-modified

# 2. 安装依赖
npm install

# 3. 启动项目
npm run dev
```

访问：http://localhost:3000

## 查看修改的页面 / View Modified Page

启动后，点击顶部导航栏的 **TEAM** 按钮，即可看到：
1. 新增的团队成员介绍部分（5个成员卡片）
2. 紫色的Submit Feedback按钮

## 项目特点 / Features

### 设计风格
- 🎨 赛博朋克 + 像素风格
- 💜 紫色和粉色主题
- ✨ 流畅的动画效果
- 📱 完全响应式设计

### 技术特点
- ⚡ Vue 3 + Vite (快速开发)
- 🌐 Globe.GL 3D地球可视化
- 🎯 Vue Router 路由管理
- 💅 自定义CSS动画

### 功能特点
- 🌍 全球空气质量实时监测
- 📊 伦敦地图可视化（占位）
- 📝 社区反馈系统
- 👥 团队成员展示
- 🔐 管理员模式（密码：ecovision2024）

## 目录说明 / Directory Structure

```
ecovision-vue-modified/
├── src/
│   ├── views/           # 页面组件
│   │   ├── Home.vue     # 首页（3D地球）
│   │   ├── Page2.vue    # ABOUT页面
│   │   ├── Map.vue      # MAP页面
│   │   └── Page4.vue    # TEAM页面 ⭐ 已修改
│   ├── router/          # 路由配置
│   ├── assets/          # 静态资源
│   ├── App.vue          # 根组件
│   └── main.js          # 入口文件
├── index.html           # HTML模板
├── package.json         # 依赖配置
└── vite.config.js       # Vite配置
```

## 核心修改代码位置 / Key Changes Location

📁 **文件：** `src/views/Page4.vue`

**修改内容：**

1. **第36-103行：** 新增团队成员介绍部分
   ```vue
   <div class="section team-intro-section">
     <div class="box">
       <h2>Meet Our Team Members</h2>
       <div class="team-members-grid">
         <!-- 5个成员卡片 -->
       </div>
     </div>
   </div>
   ```

2. **第573-654行：** 团队成员卡片样式
   ```css
   .team-members-grid { /* 网格布局 */ }
   .member-card { /* 卡片样式 */ }
   .member-avatar { /* 圆形头像 */ }
   ```

3. **第636-656行：** 按钮颜色修改
   ```css
   .submit-btn {
     background: #a855f7;  /* 改为紫色 */
     border: 3px solid #fff;
     box-shadow: 5px 5px 0 #000;
   }
   ```

## 开发命令 / Development Commands

```bash
# 安装依赖
npm install

# 启动开发服务器 (http://localhost:3000)
npm run dev

# 构建生产版本
npm run build

# 预览生产版本
npm run preview
```

## 浏览器要求 / Browser Requirements

- ✅ Chrome 90+
- ✅ Firefox 88+
- ✅ Safari 14+
- ✅ Edge 90+

## 常见问题 / FAQ

**Q: 为什么首页加载很慢？**
A: 首页使用了Globe.GL 3D地球，需要加载较大的纹理贴图。首次加载可能需要几秒钟。

**Q: 如何访问管理员模式？**
A: 在TEAM页面的反馈部分，点击"ADMIN MODE"按钮，输入密码：`ecovision2024`

**Q: 可以修改团队成员信息吗？**
A: 可以！编辑 `src/views/Page4.vue` 文件中的第36-103行，修改成员信息即可。

**Q: 按钮颜色可以自定义吗？**
A: 可以！在 `src/views/Page4.vue` 的样式部分（第636行附近）修改 `.submit-btn` 的 `background` 属性。

## 部署建议 / Deployment

### 推荐平台：
- **Vercel** - 自动部署，最简单
- **Netlify** - 功能丰富
- **GitHub Pages** - 免费托管

### 部署步骤（Vercel）：
1. 安装 Vercel CLI: `npm i -g vercel`
2. 运行: `vercel`
3. 按照提示完成部署

## 技术支持 / Support

如有问题，请检查：
1. Node.js 版本是否 >= 16
2. 依赖是否正确安装
3. 端口3000是否被占用

## License

MIT License - 可自由使用和修改
