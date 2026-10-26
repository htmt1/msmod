# Moon Wallet v0.2.0（試験版）

Moon Sweepers用の個人ウォレット・送金Modです。Windows 64bit向けです。

## ダウンロード

[ZIPをダウンロード](https://github.com/htmt1/msmod/raw/refs/heads/moonwallet-v0.2.0-download/downloads/MoonWallet-v0.2.0.zip)

リンクから保存できない場合は [ZIPのページ](downloads/MoonWallet-v0.2.0.zip) を開き、「Download raw file」または「View raw」を選んでください。非公開リポジトリの場合はアクセス権のあるGitHubアカウントでログインしてください。

## 導入

1. ゲームを終了します。最初はテスト用セーブを使ってください。
2. ZIP内の `version.dll` と `MoonWallet` フォルダーを、`MoonSweepers.exe` と同じフォルダーにコピーします。すでに `version.dll` がある場合は上書きしないでください。
3. Steamからゲームを起動します。ウォレット画面が開いたことを確認してからセーブを読み込みます。F8で表示・非表示を切り替えられます。

別のEXEの実行やCodexの環境設定・Publish操作は不要です。BepInEx用ではなく専用自動ローダー方式です。ホストと参加者の全員に同じ版を配置してください。初回導入時の既存共有残高はホストへ引き継ぎます。

**実機での自動読み込みとマルチプレイ動作は未確認です。** ビルド、台帳・保存データの単体テスト、ローダーの引数転送テストは通過しています。ゲーム本体・UnityPlayerの全体ファイルはこの開発環境で実行していません。

ZIPには日本語の詳細手順とソースコードを同梱しています。ゲーム本体のDLL・メタデータは含みません。提供されたゲームビルド専用で、別ビルドでは適用を拒否します。

SHA-256: `30e203b3208c07af6e140167df2c7fa1a8144eaa6df9cd401e2e726a7485edc3`
