# GuruguruTanque  🚀

## プロジェクト概要
**GuruguruTanque** はWindowsで動作する2人ゲームです。ゲームコントローラの使用を前提としています。
プレイヤーは１対１で、コントローラの両スティックを回転させて移動し、L2,R2ボタンの砲撃を相手に当てた方が勝ちになります。

実際のゲームの画面や詳細な工夫点は以下のリンクからご確認いただけます。
[🔗 GuruguruTanque ゲーム詳細（Notionページ）](https://picturesque-kayak-ac4.notion.site/1a6281634a168042800afab058862bde?pvs=4)

## 目次
- [使用技術](#使用技術)
- [リポジトリ構成](#リポジトリ構成)
- [工夫点](#工夫点)
- [各工夫点に関するプログラムの位置](#各工夫点に関するプログラムの位置)

## 使用技術
- DirectX12
- DirectXTex
- XAudio2
- C++

## リポジトリ構成(フォルダのみ記述)
```
Include/
├── Actor/
│   ├── MeshActor/
│   └── SceneActor/
├── Input/
├── Mesh/
└── Shader/

Source/
├── Actor/
│   ├── MeshActor/
│   └── SceneActor/
├── Input/
├── Mesh/
└── Shader/
```

## 工夫点
- **`Actor`クラスの導入**
  - 全オブジェクトの基底クラスとして`Actor`を実装し、メインループ(`Game`クラス)で全`Actor`の更新,破棄を管理するようにした
  - 各オブジェクトにActorを継承させることで個別の処理を実装しつつ、メインループで統一的に扱えるようになった
  - システム(メインループ)のコードを変更せず、オブジェクトの種類を増やせるようになり、拡張性の高い設計になった

- **`SceneActor`クラスの導入**
  - `SceneActor`を継承したシーンクラスと、`Game`によるシーン更新や遷移機能を実装した
  - シーン共通のインターフェース(`Initialize()` ,`Finalize()`)とGameによるシーン管理によって、シーン単位でのゲームデザインが可能になり、コードのメンテナンス性が向上した

- **映像再生機能の疑似的な実装**
  - イベントで本作品を出展予定だったが、mp3対応の時間が不足していた
  - 既存の環境で可能な方法を考え「連番PNG＋サウンド再生」で疑似的に実装した

- **独自3DデータフォーマットFBFIXの採用**
  - 自機の3Dモデルは元々FBXファイル。しかし、FBXには開発に不要なデータもある
  - そこで①：必要データのみ持つFBFIXの設計、②：FBXからFBFIXへ変換するコンバータの開発、③：FBFIXに対応したクラスの開発　を行った
  - ③は具体的に１：FBFIXのメッシュを保持するクラス*(`FileMesh`)、２：*を保持するActor継承クラス(`FileMeshActor`)を実装した

## 各工夫点に関するプログラムの位置
- **Actorクラスの導入**
  - Actorクラス：
    - Include or Source → Actor → Actor.h,cpp
  - Actorを継承したクラス：
    - Include or Source → ActorフォルダのActor.h,cpp以外のファイル
- **シーンクラスの導入**
  - SceneActorクラス：
    - Include or Source → Actor → SceneActor → SceneActor.h,cpp
  - SceneActorを継承したシーンクラス：
    - Include or Source → Actor → SceneActorフォルダのSceneActor以外のファイル
- **映像再生機能の疑似的な実装**
  - SpriteSequenceActorクラス：
    - Include or Source → Actor → SpriteSequenceActor.h,cpp
  - SpriteSequenceActorを用いて疑似的な映像を再現するプログラム：
    - Source → Actor → SceneActor → TitleSceneActor.cpp 36-63行目
    - Source → Actor → SceneActor → ResultSceneActor.cpp 37-82行目
- **独自3DデータフォーマットFBFIXの採用**
  - Meshクラス：
    - Include or Source → Mesh → Mesh.h,cpp
  - MeshActorクラス：
    - Include or Source → Actor → MeshActor → MeshActor.h,cpp
  - FileMeshクラス：
    - Include or Source → Mesh → FileMesh.h,cpp
  - FileMeshActorクラス：
    - Include or Source → Actor → MeshActor → FileMeshActor.h,cpp
